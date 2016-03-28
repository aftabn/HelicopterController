package com.aftabnarsimhan.helicoptercontroller;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
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
    private static final String deviceName = "HC-06";

    BluetoothAdapter mBluetoothAdapter;
    BluetoothSocket mmSocket;
    BluetoothDevice mmDevice;
    OutputStream mmOutputStream;
    InputStream mmInputStream;
    Thread workerThread;
    byte[] readBuffer;
    StringBuilder responseBuffer;
    volatile boolean stopWorker;

    private static HelicopterManager helicopterManager;
    private Timer updateTimer;

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

        helicopterManager = new HelicopterManager();
        updateTimer = new Timer();

    }

    @Override
    public void onResume() {
        super.onResume();
        //final Handler handler = new Handler();

        updateTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (mmSocket != null && mmSocket.isConnected() && helicopterManager.isPidEnabled) {
                    helicopterManager.updateValues(yawSetPointRate, tiltSetPointRate);
                }
            }
        }, 0, 200);
    }

    @Override
    public void onPause() {
        super.onPause();

        updateTimer.cancel();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_search:
                if (mmSocket == null || !mmSocket.isConnected()) {
                    try {
                        connectToBluetoothDevice(deviceName);
                        helicopterManager.connect(mmSocket, mmOutputStream);
                    }
                    catch (IOException ex) {
                        Toast.makeText(this, "Error connecting to " + deviceName,
                                Toast.LENGTH_SHORT).show();
                    }
                } else {
                    try {
                        closeBluetoothConnection();
                    }
                    catch (IOException ex) {
                        Toast.makeText(this, "Error disconnecting from " + deviceName,
                                Toast.LENGTH_SHORT).show();
                    }
                }
                return true;

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

        if (toggle != null) {
            toggle.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    if (mmSocket != null) {
                        if (isChecked) {
                            if (mmSocket.isConnected() && !helicopterManager.isPidEnabled) {
                                helicopterManager.enablePid();
                            }
                        } else {
                            if (mmSocket.isConnected() && helicopterManager.isPidEnabled) {
                                helicopterManager.disablePid();
                            }
                        }
                    }
                }
            });
        }
    }

    private void connectToBluetoothDevice(String deviceName) throws IOException {
        findBluetoothDevice(deviceName);
        openBluetoothConnection();
    }

    private void findBluetoothDevice(String deviceName) {
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
                if(device.getName().equals(deviceName))
                {
                    mmDevice = device;
                    break;
                }
            }
        }
        Log.d(TAG, "Found device" + deviceName);
    }

    private void openBluetoothConnection() throws IOException {
        UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); //Standard SerialPortService ID
        mmSocket = mmDevice.createRfcommSocketToServiceRecord(uuid);
        mmSocket.connect();
        mmOutputStream = mmSocket.getOutputStream();
        mmInputStream = mmSocket.getInputStream();

        beginListenForData();

        getSupportActionBar().setSubtitle("Connected");
    }

    void beginListenForData() {
        stopWorker = false;
        readBuffer = new byte[1024];
        responseBuffer = new StringBuilder();
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

                        responseBuffer.append(incoming);

                        String response = responseBuffer.toString();

                        if (response.contains("OK\r\n") || response.contains("ERROR\r\n")) {
                            helicopterManager.receivedPackets.add(response);
                            responseBuffer.setLength(0);
                        }
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

    void closeBluetoothConnection() throws IOException {
        stopWorker = true;
        mmOutputStream.close();
        mmInputStream.close();
        mmSocket.close();
        getSupportActionBar().setSubtitle("Disconnected");
    }
}
