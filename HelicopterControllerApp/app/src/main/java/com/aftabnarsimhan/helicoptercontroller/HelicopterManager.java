package com.aftabnarsimhan.helicoptercontroller;

import android.bluetooth.BluetoothSocket;
import android.os.SystemClock;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Created by Aftab on 3/26/2016.
 */
public class HelicopterManager {

    public static final String STR_Ack = "OK\r\n";
    public static final String STR_Nack = "ERROR\r\n";

    private static final String STR_AngleSetPointCommand = "SP";
    private static final String STR_AngleCommand = "A";
    private static final String STR_PidControlCommand = "PID";
    private static final String STR_OnArg = "ON";
    private static final String STR_OffArg = "OFF";

    private static final int defaultTimeout = 3000;

    public static final int YAW_CHANNEL = 0;
    public static final int TILT_CHANNEL = 1;

    private static BluetoothSocket mmSocket;
    private static OutputStream mmOutputStream;

    private final Lock sendDataLock = new ReentrantLock();
    private final Lock receivedPacketsLock = new ReentrantLock();

    public boolean isPidEnabled =  false;

    public List<String> receivedPackets;
    public double currentYawSetPoint;
    public double currentYawAngle;
    public double currentTiltSetPoint;
    public double currentTiltAngle;


    public int dataCount = 0;
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

        clearReceivedPacketsBuffer();

        yawSetPointsData.clear();
        yawAnglesData.clear();
        tiltSetPointsData.clear();
        tiltAnglesData.clear();
    }

    public void updateValues(double yawSetPointRate, double tiltSetPointRate) {
        double tiltSetPoint = currentTiltSetPoint + tiltSetPointRate;
        double constrainedTiltSetPoint = Math.min(23, tiltSetPoint);

        setAngleSetPoint(YAW_CHANNEL, currentYawSetPoint + yawSetPointRate);
        setAngleSetPoint(TILT_CHANNEL, constrainedTiltSetPoint);

        currentYawSetPoint = getAngleSetPoint(YAW_CHANNEL);
        currentYawAngle = getCurrentAngle(YAW_CHANNEL);
        currentTiltSetPoint = getAngleSetPoint(TILT_CHANNEL);
        currentTiltAngle = getCurrentAngle(TILT_CHANNEL);

        yawSetPointsData.add(currentYawSetPoint);
        yawAnglesData.add(currentYawAngle);
        tiltSetPointsData.add(currentTiltSetPoint);
        tiltAnglesData.add(currentTiltAngle);
        dataCount++;
    }

    private void resetChartData() {
        yawSetPointsData.clear();
        yawAnglesData.clear();
        tiltSetPointsData.clear();
        tiltAnglesData.clear();
        dataCount = 0;
    }

    private Packet sendCommand(String command) {
        sendDataLock.lock();
        Packet packet;

        try {
            clearReceivedPacketsBuffer();
            command += "\r\n";

            try {
                mmOutputStream.write(command.getBytes());
            } catch (IOException ex) {}


            try {
                waitForResponsePacket();
                packet = Packet.FromString(receivedPackets.get(0));
            } catch (TimeoutException ex) {
                packet = null;
            }
        } finally {
            sendDataLock.unlock();
        }

        return packet;
    }

    public void enablePid() {
        if (!isPidEnabled)
        {
            String command = STR_PidControlCommand + " " + STR_OnArg;
            sendCommand(command);
            isPidEnabled = true;
            resetChartData();
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

    private double getCurrentAngle(int channel) {
        String command = STR_AngleCommand + " " + channel;
        Packet response = sendCommand(command);

        return response != null ? Double.parseDouble(response.ReturnValue) : Double.NaN;
    }

    private double getAngleSetPoint(int channel) {
        String command = STR_AngleSetPointCommand + " " + channel;
        Packet response = sendCommand(command);

        return response != null ? Double.parseDouble(response.ReturnValue) : Double.NaN;
    }

    private void setAngleSetPoint(int channel, double setPoint) {
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
            throw new TimeoutException("Timeout waiting for response packet");
        }
    }

    private void clearReceivedPacketsBuffer() {
        receivedPacketsLock.lock();
        try {
            receivedPackets.clear();
        } finally {
            receivedPacketsLock.unlock();
        }
    }

    public void addReceivedPacket(String packetString) {
        receivedPacketsLock.lock();
        try {
            receivedPackets.add(packetString);
        } finally {
            receivedPacketsLock.unlock();
        }
    }
}
