package com.aftabnarsimhan.helicoptercontroller;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.util.Log;

public class Utils {

    static final String TAG = "HELICOPTER_LOG";

    public static void log(String message) {
        if (message != null) Log.i(TAG, message);
    }

    public static String getPreference(Context context, String item) {
        final SharedPreferences settings = PreferenceManager.getDefaultSharedPreferences(context);
        return settings.getString(item, TAG);
    }
}
