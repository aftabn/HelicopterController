package com.aftabnarsimhan.helicoptercontroller;

import android.bluetooth.BluetoothSocket;
import android.os.SystemClock;

import com.aftabnarsimhan.helicoptercontroller.hardware.Packet;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeoutException;

/**
 * Created by Aftab on 3/26/2016.
 */
public class HelicopterManager {

    public static final String STR_Ack = "OK\r\n";
    public static final String STR_Nack = "ERROR\r\n";

    private static final String STR_IdentityCommand = "*IDN?";
    private static final String STR_FirmwareVersionCommand = "VER";
    private static final String STR_ChangeLogCommand = "CHANGELOG";
    private static final String STR_ProportionalGainCommand = "P";
    private static final String STR_IntegralGainCommand = "I";
    private static final String STR_DerivativeGainCommand = "D";
    private static final String STR_IntegralWindupCommand = "IW";
    private static final String STR_OutputRateLimitCommand = "RL";
    private static final String STR_PidLoopIntervalCommand = "LI";
    private static final String STR_MotorOutputCommand = "O";
    private static final String STR_AngleSetPointCommand = "SP";
    private static final String STR_AngleCommand = "A";
    private static final String STR_ZeroEncoderAngleCommand = "Z";
    private static final String STR_DirectionCommand = "DC";
    private static final String STR_MotorDriverCommand = "DV";
    private static final String STR_DacVoltageCommand = "DAC";
    private static final String STR_FrequencyOutputCommand = "F";
    private static final String STR_AdcReadCommand = "ADC";
    private static final String STR_PidControlCommand = "PID";
    private static final String STR_SafetyCommand = "SAFETY";
    private static final String STR_VerboseCommand = "VERBOSE";
    private static final String STR_OnArg = "ON";
    private static final String STR_OffArg = "OFF";

    private static final int defaultTimeout = 3000;

    public static final int YAW_CHANNEL = 0;
    public static final int TILT_CHANNEL = 1;

    private static BluetoothSocket mmSocket;
    private static OutputStream mmOutputStream;

    public boolean isPidEnabled =  false;

    public List<String> receivedPackets;
    public double currentYawSetPoint;
    public double currentYawAngle;
    public double currentTiltSetPoint;
    public double currentTiltAngle;

    public List<Double> yawSetPointsData;
    public List<Double> yawAnglesData;
    public List<Double> tiltSetPointsData;
    public List<Double> tiltAnglesData;

    public HelicopterManager() {
        receivedPackets = new ArrayList<>();

        yawSetPointsData = new ArrayList<>();
        yawAnglesData = new ArrayList<>();
        tiltSetPointsData = new ArrayList<>();
        tiltAnglesData = new ArrayList<>();
    }

    public void connect(BluetoothSocket socket, OutputStream outputStream) {
        mmSocket = socket;
        mmOutputStream = outputStream;

        currentYawSetPoint = getAngleSetPoint(YAW_CHANNEL);
        currentYawAngle = getCurrentAngle(YAW_CHANNEL);

        currentTiltSetPoint = getAngleSetPoint(TILT_CHANNEL);
        currentTiltAngle = getCurrentAngle(TILT_CHANNEL);
    }

    public void updateValues(double yawSetPointRate, double tiltSetPointRate) {
        setAngleSetPoint(YAW_CHANNEL, currentYawSetPoint + yawSetPointRate);
        setAngleSetPoint(TILT_CHANNEL, currentTiltSetPoint + tiltSetPointRate);

        currentYawSetPoint = getAngleSetPoint(YAW_CHANNEL);
        currentYawAngle = getCurrentAngle(YAW_CHANNEL);

        currentTiltSetPoint = getAngleSetPoint(TILT_CHANNEL);
        currentTiltAngle = getCurrentAngle(TILT_CHANNEL);
    }

    public Packet sendCommand(String command) {
        receivedPackets.clear();

        command += "\r\n";

        try
        {
            mmOutputStream.write(command.getBytes());
            Utils.log("Sending command: <" + command + ">");
        }
        catch (IOException ex) {}


        Packet packet;

        try
        {
            waitForResponsePacket();
            packet = Packet.FromString(receivedPackets.get(0));
        }
        catch (TimeoutException ex) {
            packet = null;
        }

        return packet;
    }

    public void enablePid() {
        if (!isPidEnabled)
        {
            String command = STR_PidControlCommand + " " + STR_OnArg;
            sendCommand(command);
            isPidEnabled = true;
        }
    }

    public void disablePid() {
        if (isPidEnabled)
        {
            String command = STR_PidControlCommand + " " + STR_OffArg;
            sendCommand(command);
            isPidEnabled = false;
        }
    }

    public double getCurrentAngle(int channel) {
        String command = STR_AngleCommand + " " + channel;
        Packet response = sendCommand(command);

        return response != null ? Double.parseDouble(response.ReturnValue) : Double.NaN;
    }

    public double getAngleSetPoint(int channel) {
        String command = STR_AngleSetPointCommand + " " + channel;
        Packet response = sendCommand(command);

        return response != null ? Double.parseDouble(response.ReturnValue) : Double.NaN;
    }

    public void setAngleSetPoint(int channel, double setPoint) {
        String command = STR_AngleSetPointCommand + " " + channel + " " + setPoint;
        sendCommand(command);
    }

    private void waitForResponsePacket() throws TimeoutException {
        long startTime = System.currentTimeMillis();
        while(receivedPackets.size() < 1 && (System.currentTimeMillis() - startTime) < defaultTimeout)
        {
            SystemClock.sleep(5);
        }

        if (receivedPackets.size() < 1) {
            Utils.log("Timeout waiting for response packet");
            throw new TimeoutException("Timeout waiting for response packet");
        }
    }
}
