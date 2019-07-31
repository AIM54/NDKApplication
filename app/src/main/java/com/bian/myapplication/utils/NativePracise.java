package com.bian.myapplication.utils;

public class NativePracise {
    static {
        System.loadLibrary("NdkTest");
    }

    public static native void test();
}
