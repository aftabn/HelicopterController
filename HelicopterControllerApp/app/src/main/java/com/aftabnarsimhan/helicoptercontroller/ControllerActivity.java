package com.aftabnarsimhan.helicoptercontroller;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.ToggleButton;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

public class ControllerActivity extends AppCompatActivity {

    private static final String TAG = "ControllerActivity";
    private static final String DEVICE_NAME = "DEVICE_NAME";

    BluetoothAdapter mBluetoothAdapter;
    BluetoothSocket mmSocket;
    BluetoothDevice mmDevice;
    OutputStream mmOutputStream;
    InputStream mmInputStream;
    Thread workerThread;
    byte[] readBuffer;
    int readBufferPosition;
    volatile boolean stopWorker;

    private static HelicopterManager helicopterManager;
    private Thread helicopterUpdateThread;

    private double yawSetPointRate = 0;
    private double tiltSetPointRate = 0;

    private RelativeLayout layout_joystick;
    private TextView yawSpTextView, yawAngleTextView, tiltSpTextView, tiltAngleTextView;
    private JoyStick joyStick;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initializeJoystick();
        initializePidToggleButton();
        initializeBluetooth(savedInstanceState);
        helicopterManager = new HelicopterManager(mmSocket, mmOutputStream);
    }

    @Override
    public void onResume() {
        super.onResume();
        //final Handler handler = new Handler();

        Timer timer = new Timer();
        timer.schedule(new TimerTask()
        {
            @Override
            public void run()
            {
                if (HelicopterManager.isConnected()) {
                    helicopterManager.updateValues(yawSetPointRate, tiltSetPointRate);
                }
            }
        }, 0, 200);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_settings:
                final Intent intent = new Intent(this, SettingsActivity.class);
                startActivity(intent);
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void initializeJoystick() {
        yawSpTextView = (TextView)findViewById(R.id.yawSetPointTextView);
        yawAngleTextView = (TextView)findViewById(R.id.yawAngleTextView);
        tiltSpTextView = (TextView)findViewById(R.id.tiltSetPointTextView);
        tiltAngleTextView = (TextView)findViewById(R.id.tiltAngleTextView);

        layout_joystick = (RelativeLayout)findViewById(R.id.layout_joystick);

        joyStick = new JoyStick(getApplicationContext()
                , layout_joystick, R.drawable.image_button);
        joyStick.setStickSize(250, 250);
        joyStick.setLayoutSize(800, 800);
        joyStick.setLayoutAlpha(150);
        joyStick.setStickAlpha(100);
        joyStick.setOffset(90);
        joyStick.setMinimumDistance(10);

        layout_joystick.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View view, MotionEvent motionEvent) {
                joyStick.drawStick(motionEvent);

                if(motionEvent.getAction() == MotionEvent.ACTION_DOWN
                        || motionEvent.getAction() == MotionEvent.ACTION_MOVE) {
                    yawSetPointRate = Math.min(Math.max(-5, joyStick.getX()), 5);
                    tiltSetPointRate = Math.min(Math.max(-2, joyStick.getX()), 2);
                    yawSpTextView.setText("SP : " + String.valueOf(helicopterManager.currentYawSetPoint));
                    yawAngleTextView.setText("Angle : " + String.valueOf(helicopterManager.currentYawAngle));
                    tiltSpTextView.setText("SP : " + String.valueOf(helicopterManager.currentTiltSetPoint));
                    tiltAngleTextView.setText("Angle : " + String.valueOf(helicopterManager.currentTiltAngle));
                } else if (motionEvent.getAction() == MotionEvent.ACTION_UP) {
                    yawSetPointRate = 0;
                    tiltSetPointRate = 0;
                }

                return true;
            }
        });
    }

    //TODO: Finish this once the microcontroller class is setup
    private void initializePidToggleButton() {
        ToggleButton toggle = (ToggleButton) findViewById(R.id.pidToggleButton);
        toggle.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    if (HelicopterManager.isConnected()) helicopterManager.enablePid();
                } else {
                    if (HelicopterManager.isConnected()) helicopterManager.disablePid();
                }
            }
        });
    }

    private void initializeBluetooth(Bundle savedInstanceState) {
        try {
            findBT();
            openBT();
        }
        catch (IOException ex) { }
    }

    private void findBT()
    {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if(mBluetoothAdapter == null)
        {
            //myLabel.setText("No bluetooth adapter available");
        }

        if(!mBluetoothAdapter.isEnabled())
        {
            Intent enableBluetooth = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBluetooth, 0);
        }

        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
        if(pairedDevices.size() > 0)
        {
            for(BluetoothDevice device : pairedDevices)
            {
                if(device.getName().equals("HC-06"))
                {
                    mmDevice = device;
                    break;
                }
            }
        }
        //myLabel.setText("Bluetooth Device Found");
    }

    void openBT() throws IOException {
        UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); //Standard SerialPortService ID
        mmSocket = mmDevice.createRfcommSocketToServiceRecord(uuid);
        mmSocket.connect();
        mmOutputStream = mmSocket.getOutputStream();
        mmInputStream = mmSocket.getInputStream();

        beginListenForData();

        // TODO: Set the actionbar to connected
        getSupportActionBar().setSubtitle("Connected");
    }

    void beginListenForData() {
        final Handler handler = new Handler();
        final byte delimiter = 10; //This is the ASCII code for a newline character

        stopWorker = false;
        readBufferPosition = 0;
        readBuffer = new byte[1024];
        final StringBuilder responseBuffer = new StringBuilder();
        workerThread = new Thread(new Runnable()
        {
            public void run()
            {
                while(!Thread.currentThread().isInterrupted() && !stopWorker)
                {
                    try
                    {
                        int bytes = mmInputStream.read(readBuffer);
                        String incoming = new String(readBuffer, 0, bytes);
                        //Log.d(TAG, incoming);

                        responseBuffer.append(incoming);

                        String response = responseBuffer.toString();

                        if (response.contains("OK\r\n") || response.contains("ERROR\r\n")) {
                            helicopterManager.receivedPackets.add(response);
                            responseBuffer.setLength(0);
                        }

//                        int bytesAvailable = mmInputStream.available();
//                        if(bytesAvailable > 0)
//                        {
//                            byte[] packetBytes = new byte[bytesAvailable];
//                            mmInputStream.read(packetBytes);
//                            for(int i=0;i<bytesAvailable;i++)
//                            {
//                                byte b = packetBytes[i];
//                                if(b == delimiter)
//                                {
//                                    byte[] encodedBytes = new byte[readBufferPosition];
//                                    System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
//                                    final String data = new String(encodedBytes, "US-ASCII");
//                                    readBufferPosition = 0;
//
//                                    handler.post(new Runnable()
//                                    {
//                                        public void run()
//                                        {
//                                            myLabel.setText(data);
//                                        }
//                                    });
//                                }
//                                else
//                                {
//                                    readBuffer[readBufferPosition++] = b;
//                                }
//                            }
//                        }
                    }
                    catch (IOException ex)
                    {
                        stopWorker = true;
                    }
                }
            }
        });

        workerThread.start();
    }

    void closeBT() throws IOException
    {
        stopWorker = true;
        mmOutputStream.close();
        mmInputStream.close();
        mmSocket.close();
        getSupportActionBar().setSubtitle("Disconnected");
    }
}
