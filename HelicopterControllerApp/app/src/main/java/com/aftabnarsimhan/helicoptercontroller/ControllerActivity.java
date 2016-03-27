package com.aftabnarsimhan.helicoptercontroller;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.ToggleButton;

import com.aftabnarsimhan.helicoptercontroller.bluetooth.DeviceConnector;
import com.aftabnarsimhan.helicoptercontroller.bluetooth.DeviceData;
import com.aftabnarsimhan.helicoptercontroller.bluetooth.Utils;

import java.lang.ref.WeakReference;

public class ControllerActivity extends BaseActivity {
    private static final String DEVICE_NAME = "DEVICE_NAME";

    private static String MSG_NOT_CONNECTED;
    private static String MSG_CONNECTING;
    private static String MSG_CONNECTED;

    private static DeviceConnector connector;
    private static BluetoothResponseHandler mHandler;

    private String commandEnding;
    private String deviceName;

    private RelativeLayout layout_joystick;
    private ImageView image_joystick, image_border;
    private TextView yawSpTextView, yawAngleTextView, tiltSpTextView, tiltAngleTextView;

    private JoyStick joyStick;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initializeJoystick();
        initializePidToggleButton();
        initializeBluetooth(savedInstanceState);
    }

    @Override
    public void onStart() {
        super.onStart();
        this.commandEnding = getCommandEnding();
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
                    if (isConnected()) stopConnection();
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
                    if (super.isAdapterReady() && (connector == null)) setupConnector(device);
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
        joyStick.setStickSize(150, 150);
        joyStick.setLayoutSize(500, 500);
        joyStick.setLayoutAlpha(150);
        joyStick.setStickAlpha(100);
        joyStick.setOffset(90);
        joyStick.setMinimumDistance(50);

        layout_joystick.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View view, MotionEvent motionEvent) {
                joyStick.drawStick(motionEvent);

                if(motionEvent.getAction() == MotionEvent.ACTION_DOWN
                        || motionEvent.getAction() == MotionEvent.ACTION_MOVE) {
                    yawSpTextView.setText("SP : " + String.valueOf(joyStick.getX()));
                    tiltSpTextView.setText("SP : " + String.valueOf(joyStick.getY()));
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
                    // The toggle is enabled
                } else {
                    // The toggle is disabled
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

        if (isConnected() && (savedInstanceState != null)) {
            setDeviceName(savedInstanceState.getString(DEVICE_NAME));
        } else getSupportActionBar().setSubtitle(MSG_NOT_CONNECTED);
    }

    private boolean isConnected() {
        return (connector != null) && (connector.getState() == DeviceConnector.STATE_CONNECTED);
    }

    private void stopConnection() {
        if (connector != null) {
            connector.stop();
            connector = null;
            deviceName = null;
        }
    }

    private void startDeviceListActivity() {
        stopConnection();
        Intent serverIntent = new Intent(this, DeviceListActivity.class);
        startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
    }

    // TODO: Move this into the microcontroller class
    private String getCommandEnding() {
        String result = Utils.getPreference(this, getString(R.string.pref_commands_ending));
        if (result.equals("\\r\\n")) result = "\r\n";
        else if (result.equals("\\n")) result = "\n";
        else if (result.equals("\\r")) result = "\r";
        else result = "";
        return result;
    }

    private void setupConnector(BluetoothDevice connectedDevice) {
        stopConnection();
        try {
            String emptyName = getString(R.string.empty_device_name);
            DeviceData data = new DeviceData(connectedDevice, emptyName);
            connector = new DeviceConnector(data, mHandler);
            connector.connect();
        } catch (IllegalArgumentException e) {
            Utils.log("setupConnector failed: " + e.getMessage());
        }
    }

    // TODO: Implement this in another class that deals with the sending of bluetooth data
//    public void sendCommand(View view) {
//        if (commandEditText != null) {
//            String commandString = commandEditText.getText().toString();
//            if (commandString.isEmpty()) return;
//
//            byte[] command = commandString.getBytes();
//
//            if (command_ending != null) command = Utils.concat(command, command_ending.getBytes());
//            if (isConnected()) {
//                connector.write(command);
//                Utils.log("Sending command: " + command);
//            }
//        }
//    }

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

                        Utils.log("MESSAGE_STATE_CHANGE: " + msg.arg1);
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
                        final String readMessage = (String) msg.obj;
                        if (readMessage != null) {
                            Utils.log("Received: " + readMessage);
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
