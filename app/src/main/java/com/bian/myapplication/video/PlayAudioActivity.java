package com.bian.myapplication.video;


import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.Button;

import androidx.appcompat.app.AppCompatActivity;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.AppConstant;

import java.io.File;
import java.io.IOException;

public class PlayAudioActivity extends AppCompatActivity implements View.OnClickListener {
    private Button playAudioButton;
    private String fileUrl;
    private VideoPlayer videoPlayer;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play_audio);
        initData();
        initView();
    }

    private void initData() {
        fileUrl=getIntent().getStringExtra(AppConstant.ARG_VIDEO_PATH);
        videoPlayer=new VideoPlayer(null);
    }

    private void initView() {
        playAudioButton=findViewById(R.id.play_audio);
        playAudioButton.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()){
            case R.id.play_audio:
                videoPlayer.playerAudio(fileUrl);
                break;
        }
    }



    @Override
    protected void onDestroy() {
        super.onDestroy();
        videoPlayer.onDestroy();
    }
}
