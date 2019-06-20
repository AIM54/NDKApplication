package com.bian.myapplication.utils;

import android.graphics.Bitmap;

public class JavaUtil {
    static {
        System.loadLibrary("libswscale");
        System.loadLibrary("libswresample");
        System.loadLibrary("libavutil");
        System.loadLibrary("libavformat");
        System.loadLibrary("libavfilter");
        System.loadLibrary("libavdevice");
        System.loadLibrary("libavcodec");
        System.loadLibrary("ffmpeg-lib");
        System.loadLibrary("bian-lib");
    }

    public native Bitmap getBitMapFromVideo(String videoPath);
}
