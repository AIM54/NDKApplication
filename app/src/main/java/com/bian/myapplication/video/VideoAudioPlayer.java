package com.bian.myapplication.video;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.Surface;

import androidx.annotation.NonNull;

import com.bian.myapplication.utils.CommonLog;

public class VideoAudioPlayer {
    static {
        System.loadLibrary("swscale");
        System.loadLibrary("swresample");
        System.loadLibrary("avutil");
        System.loadLibrary("avformat");
        System.loadLibrary("avfilter");
        System.loadLibrary("avdevice");
        System.loadLibrary("avcodec");
        System.loadLibrary("VideoPlayer");
    }

    private Surface mSurface;
    HandlerThread mHandlerThread;
    private Handler videoHandler;
    public final static int PLAY_VIDEO_CODE = 111;
    public final static int PLAY_DESTORY_CODE = 119;
    public final static int PLAY_PAUSE_CODE = 120;
    private Handler.Callback mCallback = new Handler.Callback() {
        @Override
        public boolean handleMessage(@NonNull Message message) {
            switch (message.what) {
                case PLAY_VIDEO_CODE:
                    nPlayVideo((String) message.obj, mSurface);
                    break;
                case PLAY_DESTORY_CODE:
                    nDestroyPlay();
                    break;
                case PLAY_PAUSE_CODE:
                    nPauseTheVideo();
                    break;
            }
            return true;
        }
    };

    public VideoAudioPlayer(Surface holder) {
        this.mSurface = holder;
        mHandlerThread = new HandlerThread("VideoPlayer");
        mHandlerThread.start();
        videoHandler = new Handler(mHandlerThread.getLooper(), mCallback);
    }

    public void playVideo(String videoUrl) {
        sendMessage(videoUrl, PLAY_VIDEO_CODE);
    }

    public void pauseVideo() {
        sendMessage(null, PLAY_PAUSE_CODE);
    }

    public void destroVideo() {
        sendMessage(null, PLAY_DESTORY_CODE);
    }

    public <T> void sendMessage(T data, int code) {
        Message message = new Message();
        message.what = code;
        message.obj = data;
        videoHandler.sendMessage(message);
    }

    private native int nPlayVideo(String url, Surface surface);

    private native int nPauseTheVideo();

    private native int nDestroyPlay();


}
