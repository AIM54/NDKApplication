package com.bian.myapplication.video;

import android.media.MediaCodec;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.AppConstant;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.utils.VideoUtil;

import androidx.appcompat.app.AppCompatActivity;

public class VideoPlayActivity extends AppCompatActivity implements SurfaceHolder.Callback {

    private String stringVideoPath;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private MediaCodec mediaCodec;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_play);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        stringVideoPath = getIntent().getStringExtra(AppConstant.ARG_VIDEO_PATH);
        mSurfaceView = findViewById(R.id.sfv_main);
        mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.addCallback(this);
    }


    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                VideoUtil.decodeVideo(stringVideoPath, mSurfaceHolder.getSurface());

            }
        }).start();

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}
