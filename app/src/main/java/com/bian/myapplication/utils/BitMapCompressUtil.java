package com.bian.myapplication.utils;

import android.graphics.Bitmap;

public class BitMapCompressUtil {
    static {
        System.loadLibrary("turbojpeg");
        System.loadLibrary("imageCompress");
    }

    public static native int compressBitmap(Bitmap bitmap, String dstPath);
}
