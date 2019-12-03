package com.bian.myapplication.utils;

import android.content.res.AssetManager;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.Surface;

import androidx.annotation.NonNull;

import java.util.ArrayList;

public class NativePracise {
    static {
        System.loadLibrary("NdkTest");
    }

    public static native void init();

    public static native void resize(int width, int height);

    public static native void step();

    public static native void initAsserts(AssetManager assertManager);

}
