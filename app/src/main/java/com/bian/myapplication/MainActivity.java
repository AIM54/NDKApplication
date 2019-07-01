package com.bian.myapplication;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CursorAdapter;
import android.widget.ListView;
import android.widget.SimpleCursorAdapter;
import android.widget.Toast;

import com.bian.myapplication.bean.VideoBean;
import com.bian.myapplication.dialog.SelectOptionDialog;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.utils.VideoUtil;
import com.bian.myapplication.video.VideoPlayActivity;

import java.io.File;
import java.io.IOException;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.loader.app.LoaderManager;
import androidx.loader.content.CursorLoader;
import androidx.loader.content.Loader;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, LoaderManager.LoaderCallbacks<Cursor>, AdapterView.OnItemClickListener {
    private ListView mListView;
    private Button toastButton, encodeVideoBt;
    private CursorAdapter mCursorAdapter;
    private String[] videoProjects = new String[]{
            MediaStore.Video.Media._ID,
            MediaStore.Video.Media.DISPLAY_NAME,
            MediaStore.Video.Media.DATA};

    private SelectOptionDialog selectOptionDialog;
    private String mFilePath;
    private String saveVideodir;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        CommonLog.e("onCreate");
        setContentView(R.layout.activity_main);
        initView();
        initData();
    }

    private void initData() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 110);
        } else {
            saveVideodir = Environment.getExternalStorageDirectory() + File.separator + "VideoPath";
            queryVideoInfo();
        }
    }

    private void queryVideoInfo() {
        LoaderManager manager = LoaderManager.getInstance(this);
        manager.initLoader(110, null, this);
    }

    private void initView() {
        mListView = findViewById(R.id.lv_video);
        encodeVideoBt = findViewById(R.id.bt_encode_video);
        toastButton = findViewById(R.id.bt_toast);
        toastButton.setOnClickListener(this);
        encodeVideoBt.setOnClickListener(this);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == 110) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                queryVideoInfo();
            }
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.bt_encode_video:
                String videoName = null;
                try {
                    videoName = createTargetFile();
                    CommonLog.i("");
                    VideoUtil.encodeTest(videoName);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
            case R.id.bt_toast:
                VideoUtil.testSoLibrary();
                break;
        }

    }

    private String createTargetFile() throws IOException {
        String videoMenu = Environment.getExternalStorageDirectory() + File.separator + "Output";
        File menu = new File(videoMenu);
        menu.mkdirs();
        File videoFile = new File(videoMenu + File.separator + "test.avi");
        return videoFile.getAbsolutePath();
    }

    public void toastMessage(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    @NonNull
    @Override
    public Loader<Cursor> onCreateLoader(int id, @Nullable Bundle args) {
        CursorLoader cursorLoader = new CursorLoader(this, MediaStore.Video.Media.EXTERNAL_CONTENT_URI, videoProjects, null, null, null);
        return cursorLoader;
    }

    @Override
    public void onLoadFinished(@NonNull Loader<Cursor> loader, Cursor data) {
        mCursorAdapter = new SimpleCursorAdapter(this,
                R.layout.i_video_info,
                data,
                new String[]{MediaStore.Video.Media.DISPLAY_NAME},
                new int[]{R.id.tv_name}, 0);
        mListView.setAdapter(mCursorAdapter);
        mListView.setOnItemClickListener(this);
    }

    @Override
    public void onLoaderReset(@NonNull Loader<Cursor> loader) {

    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        Cursor cursor = (Cursor) mCursorAdapter.getItem(position);
        mFilePath = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.DATA));
        selectOptionDialog = new SelectOptionDialog(this, new VideoOpertionListener());
        selectOptionDialog.show();
    }

    public class VideoOpertionListener implements AdapterView.OnItemClickListener {

        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            CommonLog.i("the clicked postion:" + position);
            selectOptionDialog.dismiss();
            switch (position) {
                case 0:
                    playVideo();
                    break;
                case 1:
                    addTextToVideo(mFilePath, "AIM54");
                    break;
            }
        }
    }

    private void playVideo() {
        Intent playVideoIt = new Intent(this, VideoPlayActivity.class);
        playVideoIt.putExtra(VideoPlayActivity.ARG_VIDEO_PATH, mFilePath);
        startActivity(playVideoIt);
    }

    private void addTextToVideo(String filePath, String markText) {
        String tempPath = saveVideodir + File.separator + "VideoInfo_" + System.currentTimeMillis() + ".avi";
        VideoUtil.addTextToVideo(filePath, markText, tempPath);
    }
}
