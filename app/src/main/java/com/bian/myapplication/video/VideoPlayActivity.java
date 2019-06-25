package com.bian.myapplication.video;

import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.utils.VideoUtil;

import androidx.appcompat.app.AppCompatActivity;

public class VideoPlayActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    public static final String ARG_VIDEO_PATH = "ARG_VIDEO_PATH";
    private String stringVideoPath;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_play);
        stringVideoPath = getIntent().getStringExtra(ARG_VIDEO_PATH);
        mSurfaceView=findViewById(R.id.sfv_main);
        mSurfaceHolder=mSurfaceView.getHolder();
        mSurfaceHolder.addCallback(this);
    }



    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                VideoUtil.decodeVideo(stringVideoPath,mSurfaceHolder.getSurface());
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
