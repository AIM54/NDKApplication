package com.bian.myapplication.video;

import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.Surface;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;

import com.bian.myapplication.utils.CommonLog;

import java.io.File;
import java.io.IOException;

public class VideoPlayer {
    private Surface mSurface;
    HandlerThread mHandlerThread;
    private Handler videoHandler;
    public final static int OPEN_VIDEO_CODE = 110;
    public final static int PLAY_VIDEO_CODE = 111;
    public final static int PLAY_AUDIO_CODE = 112;
    public final static int PLAY_DESTORY_CODE = 119;
    private Handler.Callback mCallback = new Handler.Callback() {
        @Override
        public boolean handleMessage(@NonNull Message message) {
            switch (message.what) {
                case OPEN_VIDEO_CODE:
                    onpenVideo((String) message.obj);
                    break;
                case PLAY_VIDEO_CODE:
                    CommonLog.i("beginPlayVideo:"+message.obj);
                    playVideo((String) message.obj,mSurface);
                    break;
                case PLAY_AUDIO_CODE:
                    String ouputUrl = createOutputFile();
                    CommonLog.i(ouputUrl);
                    playAudio((String) message.obj, ouputUrl);
                    break;
                case PLAY_DESTORY_CODE:
                    destory();
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

    public VideoPlayer(Surface holder) {
        this.mSurface = holder;
        mHandlerThread = new HandlerThread("VideoPlayer");
        mHandlerThread.start();
        videoHandler = new Handler(mHandlerThread.getLooper(), mCallback);
    }

    public void playerVideo(String url) {
        sendMessage(url, PLAY_VIDEO_CODE);
    }

    public void playerAudio(String url) {
        String ouputUrl = createOutputFile();
        CommonLog.i(ouputUrl);
        playAudio(url, ouputUrl);
    }

    private String createOutputFile() {
        File menuFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "DecodeFile");
        if (!menuFile.exists()) {
            menuFile.mkdirs();
        }
        File targetFile = new File(menuFile.getAbsolutePath() + File.separator + "1234.pcm");
        if (!targetFile.exists()) {
            try {
                targetFile.createNewFile();
                return targetFile.getAbsolutePath();
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {
            return targetFile.getAbsolutePath();
        }
        return "";
    }

    public native void onpenVideo(String url);

    private native void playVideo(String url, Surface surface);

    private native void playAudio(String filePath, String outputUrl);

    private native void destory();


    public <T> void sendMessage(T data, int code) {
        Message message = new Message();
        message.what = code;
        message.obj = data;
        videoHandler.sendMessage(message);
    }


    public void prapareForVideo(String mFileString) {
        sendMessage(mFileString, OPEN_VIDEO_CODE);
    }

    public void onDestroy() {
        sendMessage(null, PLAY_DESTORY_CODE);
    }
}
