package com.bian.myapplication.utils;

import android.view.Surface;

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



    public static native void testSoLibrary();

    public static native void decodeVideo(String filePath, Surface holder);

    public static native void addTextToVideo(String filePath, String markText, String tempPath);

    public static void log(String infor) {
        CommonLog.i(infor);
    }

    public static native void encodePic(String filePath,String dstPath);
}
