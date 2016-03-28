package com.aftabnarsimhan.helicoptercontroller;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.RelativeLayout;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.Viewport;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

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
    private static final int INT_UpdateInterval = 250;

    // Variables for the PID Chart
    private final Handler mChartHandler = new Handler();
    private Runnable mChartTimer;
    private LineGraphSeries<DataPoint> mYawAngleSeries;
    private LineGraphSeries<DataPoint> mYawSetPointSeries;
    private LineGraphSeries<DataPoint> mTiltAngleSeries;
    private LineGraphSeries<DataPoint> mTiltSetPointSeries;

    // Bluetooth communication
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
    private JoyStick joyStick;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initializePidChart();
        initializeJoystick();
        initializePidToggleButton();

        helicopterManager = new HelicopterManager();
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();

        disconnect();
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
                    connect(deviceName);
                } else {
                    disconnect();
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

    private void initializePidChart() {
        GraphView graph = (GraphView) findViewById(R.id.graph);
        graph.setTitle("PID Graph");

        Viewport viewport = graph.getViewport();
        viewport.setScalable(true);
        //viewport.setScrollable(true);

        mYawAngleSeries = new LineGraphSeries<>();
        mYawSetPointSeries = new LineGraphSeries<>();
        mTiltAngleSeries = new LineGraphSeries<>();
        mTiltSetPointSeries = new LineGraphSeries<>();

        mYawAngleSeries.setColor(Color.parseColor("#FF8C00"));
        mYawSetPointSeries.setColor(Color.parseColor("#FFA500"));
        mTiltAngleSeries.setColor(Color.parseColor("#4682B4"));
        mTiltSetPointSeries.setColor(Color.parseColor("#ADD8E6"));

        graph.addSeries(mYawAngleSeries);
        graph.addSeries(mYawSetPointSeries);
        graph.addSeries(mTiltAngleSeries);
        graph.addSeries(mTiltSetPointSeries);
    }

    private void initializeJoystick() {
        layout_joystick = (RelativeLayout)findViewById(R.id.layout_joystick);

        joyStick = new JoyStick(getApplicationContext()
                , layout_joystick, R.drawable.image_button);
        joyStick.setStickSize(250, 250);
        joyStick.setLayoutSize(800, 800);
        joyStick.setLayoutAlpha(150);
        joyStick.setStickAlpha(100);
        joyStick.setOffset(90);
        joyStick.setMinimumDistance(30);

        layout_joystick.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View view, MotionEvent motionEvent) {
                joyStick.drawStick(motionEvent);

                if(motionEvent.getAction() == MotionEvent.ACTION_DOWN
                        || motionEvent.getAction() == MotionEvent.ACTION_MOVE) {
                    yawSetPointRate = Math.min(Math.max(-3, joyStick.getX()), 3);
                    tiltSetPointRate = Math.min(Math.max(-1, -1 * joyStick.getY()), 1);
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

    private void initializeHelicopterUpdateTimer(int intervalMs) {
        updateTimer = new Timer();

        updateTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (mmSocket != null && mmSocket.isConnected() && helicopterManager.isPidEnabled) {
                    helicopterManager.updateValues(yawSetPointRate, tiltSetPointRate);

                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            int count = helicopterManager.dataCount;
                            double yawAngle = helicopterManager.yawAnglesData.get(count - 1);
                            double yawSetPoint = helicopterManager.yawSetPointsData.get(count - 1);
                            double tiltAngle = helicopterManager.tiltAnglesData.get(count - 1);
                            double tiltSetPoint = helicopterManager.tiltSetPointsData.get(count - 1);

                            mYawAngleSeries.appendData(new DataPoint(count, yawAngle), true, 40);
                            mYawSetPointSeries.appendData(new DataPoint(count, yawSetPoint), true, 40);
                            mTiltAngleSeries.appendData(new DataPoint(count, tiltAngle), true, 40);
                            mTiltSetPointSeries.appendData(new DataPoint(count, tiltSetPoint), true, 40);
                        }
                    });
                }
            }
        }, 0, intervalMs);
    }

    private void connect(String deviceName) {
        Log.d(TAG, "Connecting to helicopter");
        if (mmSocket == null || !mmSocket.isConnected()) {
            findBluetoothDevice(deviceName);
            try {
                openBluetoothConnection();
            }
            catch (IOException ex) {
                Toast.makeText(getBaseContext(), "Error connecting to " + deviceName,
                        Toast.LENGTH_SHORT).show();
                Log.e(TAG, "Error trying to open bluetooth connection.", ex);
            }
        }
        initializeHelicopterUpdateTimer(INT_UpdateInterval);
        helicopterManager.connect(mmSocket, mmOutputStream);
    }

    private void disconnect() {
        Log.d(TAG, "Disconnecting from helicopter");
        if (mmSocket != null && mmSocket.isConnected()) {
            helicopterManager.disablePid();
            updateTimer.cancel();
            try {
                closeBluetoothConnection();
            }
            catch (IOException ex) {
                Toast.makeText(getBaseContext(), "Error disconnecting from " + deviceName,
                        Toast.LENGTH_SHORT).show();
                Log.e(TAG, "Error trying to close bluetooth connection.", ex);
            }
        }
    }

    private void findBluetoothDevice(String deviceName) {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if(mBluetoothAdapter == null)
        {
            Log.d(TAG, "No bluetooth adapter available");
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

                        if (response.contains(HelicopterManager.STR_Ack) || response.contains(HelicopterManager.STR_Nack)) {
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
