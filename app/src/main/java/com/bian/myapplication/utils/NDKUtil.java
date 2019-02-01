package com.bian.myapplication.utils;

import android.graphics.Bitmap;

public class NDKUtil {
    public static native int getEncrypMessage(String message);
    public static native int getAddResult(int a,int b);
}
