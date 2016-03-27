package com.aftabnarsimhan.helicoptercontroller;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.ToggleButton;

import com.aftabnarsimhan.helicoptercontroller.bluetooth.DeviceConnector;
import com.aftabnarsimhan.helicoptercontroller.bluetooth.DeviceData;
import com.aftabnarsimhan.helicoptercontroller.hardware.Packet;

import java.lang.ref.WeakReference;

public class ControllerActivity extends BaseActivity {

    private static final String TAG = "ControllerActivity";

    private static final String DEVICE_NAME = "DEVICE_NAME";
    private static BluetoothResponseHandler mHandler;
    private static String MSG_NOT_CONNECTED;
    private static String MSG_CONNECTING;
    private static String MSG_CONNECTED;
    private static String receivedDataBuffer = "";
    private String deviceName;

    private static HelicopterManager helicopterManager;
    private Runnable mCommunicationTimer;

    private double yawSetPointRate = 0;
    private double tiltSetPointRate = 0;

    private RelativeLayout layout_joystick;
    private TextView yawSpTextView, yawAngleTextView, tiltSpTextView, tiltAngleTextView;
    private JoyStick joyStick;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        helicopterManager = new HelicopterManager();

        initializeJoystick();
        initializePidToggleButton();
        initializeBluetooth(savedInstanceState);
    }

    @Override
    public void onStart() {
        super.onStart();
    }

    @Override
    public void onResume() {
        super.onResume();
        mCommunicationTimer = new Runnable() {
            @Override
            public void run() {
                //mSeries1.resetData(generateData());
                if (HelicopterManager.isConnected()) {
                    helicopterManager.updateValues(yawSetPointRate, tiltSetPointRate);
                }
                mHandler.postDelayed(this, 300);
            }
        };
        mHandler.postDelayed(mCommunicationTimer, 300);
    }

    @Override
    public void onPause() {
        mHandler.removeCallbacks(mCommunicationTimer);
        super.onPause();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putString(DEVICE_NAME, deviceName);
    }

    @Override
    public boolean onSearchRequested() {
        if (super.isAdapterReady()) startDeviceListActivity();
        return false;
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
                if (super.isAdapterReady()) {
                    if (HelicopterManager.isConnected()) HelicopterManager.stopConnection();
                    else startDeviceListActivity();
                } else {
                    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
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

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
            case REQUEST_CONNECT_DEVICE:
                // When DeviceListActivity returns with a device to connect
                if (resultCode == Activity.RESULT_OK) {
                    String address = data.getStringExtra(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
                    BluetoothDevice device = btAdapter.getRemoteDevice(address);
                    if (super.isAdapterReady() && (HelicopterManager.connector == null)) setupConnector(device);
                }
                break;
            case REQUEST_ENABLE_BT:
                // When the request to enable Bluetooth returns
                super.pendingRequestEnableBt = false;
                if (resultCode != Activity.RESULT_OK) {
                    Utils.log("BT not enabled");
                }
                break;
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
        if (mHandler == null) mHandler = new BluetoothResponseHandler(this);
        else mHandler.setTarget(this);

        MSG_NOT_CONNECTED = getString(R.string.msg_not_connected);
        MSG_CONNECTING = getString(R.string.msg_connecting);
        MSG_CONNECTED = getString(R.string.msg_connected);

        if (HelicopterManager.isConnected() && (savedInstanceState != null)) {
            setDeviceName(savedInstanceState.getString(DEVICE_NAME));
        } else getSupportActionBar().setSubtitle(MSG_NOT_CONNECTED);
    }

    private void startDeviceListActivity() {
        HelicopterManager.stopConnection();
        Intent serverIntent = new Intent(this, DeviceListActivity.class);
        startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
    }

    private void setupConnector(BluetoothDevice connectedDevice) {
        HelicopterManager.stopConnection();
        try {
            String emptyName = getString(R.string.empty_device_name);
            DeviceData data = new DeviceData(connectedDevice, emptyName);
            HelicopterManager.connector = new DeviceConnector(data, mHandler);
            HelicopterManager.connector.connect();
        } catch (IllegalArgumentException e) {
            Utils.log("setupConnector failed: " + e.getMessage());
        }
    }

    void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
        getSupportActionBar().setSubtitle(deviceName);
    }

    private static class BluetoothResponseHandler extends Handler {
        private WeakReference<ControllerActivity> mActivity;

        public BluetoothResponseHandler(ControllerActivity activity) {
            mActivity = new WeakReference<ControllerActivity>(activity);
        }

        public void setTarget(ControllerActivity target) {
            mActivity.clear();
            mActivity = new WeakReference<ControllerActivity>(target);
        }

        @Override
        public void handleMessage(Message msg) {
            ControllerActivity activity = mActivity.get();
            if (activity != null) {
                switch (msg.what) {
                    case MESSAGE_STATE_CHANGE:

                        Log.d(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
                        final ActionBar bar = activity.getSupportActionBar();
                        switch (msg.arg1) {
                            case DeviceConnector.STATE_CONNECTED:
                                bar.setSubtitle(MSG_CONNECTED);
                                break;
                            case DeviceConnector.STATE_CONNECTING:
                                bar.setSubtitle(MSG_CONNECTING);
                                break;
                            case DeviceConnector.STATE_NONE:
                                bar.setSubtitle(MSG_NOT_CONNECTED);
                                break;
                        }
                        break;

                    case MESSAGE_READ:
                        final String incoming = (String) msg.obj;
                        if (incoming != null) {
                            receivedDataBuffer += incoming;
                            Log.d(TAG, "Received: " + incoming);

                            if (receivedDataBuffer.contains(HelicopterManager.STR_Ack) ||
                                    receivedDataBuffer.contains(HelicopterManager.STR_Nack)) {
                                helicopterManager.receivedPackets.add(Packet.FromString(receivedDataBuffer));
                                Log.d(TAG, "Packet: " + receivedDataBuffer);
                                receivedDataBuffer = "";
                            }
                        }
                        break;

                    case MESSAGE_DEVICE_NAME:
                        activity.setDeviceName((String) msg.obj);
                        break;

                    case MESSAGE_WRITE:
                        // stub
                        break;

                    case MESSAGE_TOAST:
                        // stub
                        break;
                }
            }
        }
    }
}
