package com.bian.myapplication.video;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;

public class VideoPlayer {
    private SurfaceHolder mHolder;
    HandlerThread mHandlerThread;
    private Handler videoHandler;
    public final static int OPEN_VIDEO_CODE = 110;
    public final static int PAUSE_VIDEO_CODE = 111;

    private Handler.Callback mCallback = new Handler.Callback() {
        @Override
        public boolean handleMessage(@NonNull Message message) {
            switch (message.what) {
                case OPEN_VIDEO_CODE:
                    onpenVideo((String) message.obj);
                    break;
            }
            return true;
        }
    };

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

    public VideoPlayer(SurfaceHolder holder) {
        this.mHolder = holder;
        mHandlerThread = new HandlerThread("VideoPlayer");
        mHandlerThread.start();
        videoHandler = new Handler(mHandlerThread.getLooper(), mCallback);
    }

    public void playerVideo(String url) {

    }

    private native void onpenVideo(String url);

    public void prapareForVideo(String url) {
        Message message = new Message();
        message.what = OPEN_VIDEO_CODE;
        message.obj = url;
        videoHandler.sendMessage(message);
    }
}
