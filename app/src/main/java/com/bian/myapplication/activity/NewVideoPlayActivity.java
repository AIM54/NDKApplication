package com.bian.myapplication.activity;

import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.SeekBar;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.AppConstant;
import com.bian.myapplication.utils.VideoUtil;
import com.bian.myapplication.video.VideoPlayer;

import androidx.appcompat.app.AppCompatActivity;

public class NewVideoPlayActivity extends AppCompatActivity implements SurfaceHolder.Callback, View.OnClickListener {
    private SurfaceView mSurfaceView;
    private SeekBar videoSeekBar;
    private Button playVideoBt, pauseVideoBt, getAudioBt;
    private SurfaceHolder mHolder;
    private String mFileString;
    private VideoPlayer mVideoPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_new_video_play);
        initData();
        initView();

    }

    private void initData() {
        mFileString = getIntent().getStringExtra(AppConstant.ARG_VIDEO_PATH);
    }

    private void initView() {
        mSurfaceView = findViewById(R.id.surfaceView);
        videoSeekBar = findViewById(R.id.skb_video);
        playVideoBt = findViewById(R.id.bt_video_play);
        pauseVideoBt = findViewById(R.id.bt_video_pause);
        getAudioBt = findViewById(R.id.bt_get_audio);
        mHolder = mSurfaceView.getHolder();
        mHolder.addCallback(this);
        playVideoBt.setOnClickListener(this);
    }

    @Override
    public void surfaceCreated(final SurfaceHolder surfaceHolder) {
        mVideoPlayer = new VideoPlayer(surfaceHolder.getSurface());
        mVideoPlayer.onpenVideo(mFileString);
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        mVideoPlayer.onDestroy();
        mVideoPlayer = null;
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.bt_video_play:
                if (mVideoPlayer != null) {
                    mVideoPlayer.playerVideo(mFileString);
                }
                break;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mVideoPlayer.onDestroy();
    }
}
