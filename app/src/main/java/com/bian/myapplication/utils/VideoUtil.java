package com.bian.myapplication.utils;

import android.graphics.Bitmap;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

public class VideoUtil {
    static {
        System.loadLibrary("swscale");
        System.loadLibrary("swresample");
        System.loadLibrary("avutil");
        System.loadLibrary("avformat");
        System.loadLibrary("avfilter");
        System.loadLibrary("avdevice");
        System.loadLibrary("avcodec");
        System.loadLibrary("VideoPlay");
        System.loadLibrary("Bian");
    }

    public native Bitmap getBitMapFromVideo(String videoPath);


    public static native void testSoLibrary();

    public static native void decodeVideo(String filePath, Surface holder);

    public static void log(String infor) {
        CommonLog.i(infor);
    }
}
