package com.aftabnarsimhan.helicoptercontroller;

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

        // Separate the lines of the packet
        String lines[] = text.split("\\r?\\n");

        // Trim excess whitespace
        for (String line : lines) { line.trim(); }

        boolean isValid = ValidatePacket(lines);

        int linesCount = lines.length;
        packet.RawString = text;

        if (isValid) {
            packet.Command = lines[0].substring(STR_Prompt.length());
            packet.Terminator = lines[linesCount - 1];

            if (linesCount == 3)
            {
                packet.ReturnValue = lines[1];
            }
        } else {
            packet.Command = "<Invalid packet>";
            packet.ReturnValue = "-1";
        }

        return packet;
    }

    public static boolean ValidatePacket(String lines[]) {
        int linesCount = lines.length;
        if (linesCount < 2)
        {
            Log.d(TAG, "Not enough lines received in packet. Expected 2 or more and received " + linesCount);
            return false;
        }

        if (!(lines[linesCount - 1].equals("OK") || lines[linesCount - 1].equals("ERROR")))
        {
            Log.d(TAG, "Received packet did not contain an OK or ERROR");
            return false;
        }

        if (linesCount > 3)
        {
            Log.d(TAG, "Unsupported number of lines received. Expected 2 or 3 and received " + linesCount);
            return false;
        }

        return true;
    }

}
