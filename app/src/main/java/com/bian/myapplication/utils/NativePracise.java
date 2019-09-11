package com.bian.myapplication.utils;

import android.content.res.AssetManager;

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
