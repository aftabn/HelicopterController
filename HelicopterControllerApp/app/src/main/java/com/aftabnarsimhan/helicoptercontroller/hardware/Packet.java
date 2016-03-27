package com.aftabnarsimhan.helicoptercontroller.hardware;

import android.util.Log;

/**
 * Created by Aftab on 3/26/2016.
 */
public class Packet {

    private static final String TAG = "Packet";
    private static final String STR_Prompt = ">";

    public String Command;
    public String RawString;
    public String ReturnValue;
    public String Terminator;

    public static Packet FromString(String text) {
        Packet packet = new Packet();

        Log.d(TAG, "Processing packet: " + text);

        String lines[] = text.split("\\r?\\n");

        lines = PreProcessPacket(lines);
        ValidatePacket(lines);

        int linesCount = lines.length;

        packet.RawString = text;
        packet.Command = lines[0].substring(STR_Prompt.length());
        packet.Terminator = lines[linesCount - 1];

        if (linesCount == 3)
        {
            packet.ReturnValue = lines[1];
        }

        return packet;
    }

    public static String[] PreProcessPacket(String lines[]) {
        for (String line : lines) {
            line.trim();
        }

        return lines;
    }

    public static void ValidatePacket(String lines[]) {
        int linesCount = lines.length;
        if (linesCount < 2)
        {
            Log.d(TAG, "Not enough lines received in packet. Expected 2 or more and received " + linesCount);
            //throw new RuntimeException("Not enough lines received in packet. Expected 2 or more and received " + linesCount);
        }

        if (!(lines[linesCount - 1].equals("OK") || lines[linesCount - 1].equals("ERROR")))
        {
            Log.d(TAG, "Received packet did not contain an OK or ERROR");
            //throw new RuntimeException("Received packet did not contain an OK or ERROR");
        }

        if (linesCount > 3)
        {
            Log.d(TAG, "Unsupported number of lines received. Expected 2 or 3 and received " + linesCount);
            //throw new RuntimeException("Unsupported number of lines received. Expected 2 or 3 and received " + linesCount);
        }
    }

}
