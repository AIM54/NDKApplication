package com.bian.myapplication.video;

import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Button;
import android.widget.SeekBar;

import com.bian.myapplication.R;

import androidx.appcompat.app.AppCompatActivity;

public class NewVideoPlayActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    private SurfaceView mSurfaceView;
    private SeekBar videoSeekBar;
    private Button playVideoBt, pauseVideoBt, getAudioBt;
    private SurfaceHolder mHolder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_new_video_play);
        initView();
    }

    private void initView() {
        mSurfaceView = findViewById(R.id.surfaceView);
        videoSeekBar = findViewById(R.id.skb_video);
        playVideoBt = findViewById(R.id.bt_video_play);
        pauseVideoBt = findViewById(R.id.bt_video_pause);
        getAudioBt = findViewById(R.id.bt_get_audio);
        mHolder=mSurfaceView.getHolder();
        mHolder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }
}
