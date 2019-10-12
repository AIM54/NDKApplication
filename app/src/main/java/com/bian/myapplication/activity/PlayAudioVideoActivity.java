package com.bian.myapplication.activity;

import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;

import androidx.appcompat.app.AppCompatActivity;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.AppConstant;
import com.bian.myapplication.video.VideoAudioPlayer;

public class PlayAudioVideoActivity extends AppCompatActivity implements SurfaceHolder.Callback, View.OnClickListener {
    private SurfaceView surfaceView;
    private Button playBt;
    private VideoAudioPlayer videoAudioPlayer;
    private String videoUrl;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_play_audio_video);
        initView();
    }

    private void initView() {
        surfaceView = findViewById(R.id.sfv_main);
        playBt = findViewById(R.id.bt_play);
        surfaceView.getHolder().addCallback(this);
        playBt.setOnClickListener(this);
        videoUrl = getIntent().getStringExtra(AppConstant.ARG_VIDEO_PATH);
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        videoAudioPlayer = new VideoAudioPlayer(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.bt_play:
                videoAudioPlayer.playVideo(videoUrl);
                break;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        videoAudioPlayer.destroVideo();
    }
}
