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

    public static final int CODE_NEW = 1;
    public static final int CODE_DELETE = 2;
    private Handler.Callback mCallback = new Handler.Callback() {
        @Override
        public boolean handleMessage(@NonNull Message message) {
            switch (message.what) {
                case CODE_NEW:
                    nTestNew();
                    break;
                case CODE_DELETE:
                    nTestDelete();
                    break;
            }
            return true;
        }
    };
    private final HandlerThread mHandlerThread;
    private final Handler videoHandler;

    public static native void init();

    public static native void resize(int width, int height);

    public static native void step();


    public static native void initAsserts(AssetManager assertManager);

    public static native void nTestNew();

    public static native void nTestDelete();

    public NativePracise() {
        mHandlerThread = new HandlerThread("VideoPlayer");
        mHandlerThread.start();
        videoHandler = new Handler(mHandlerThread.getLooper(), mCallback);
    }

    public void newObject() {
        CommonLog.i("newObject");
        sendMessage(null, CODE_NEW);
    }

    public void deleteObject() {
        CommonLog.i("deleteObject");
        sendMessage(null, CODE_DELETE);
    }

    public <T> void sendMessage(T data, int code) {
        Message message = new Message();
        message.what = code;
        message.obj = data;
        videoHandler.sendMessage(message);
    }
}
