package com.bian.myapplication;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CursorAdapter;
import android.widget.ListView;
import android.widget.SimpleCursorAdapter;
import android.widget.Toast;

import com.bian.myapplication.activity.ImageActivity;
import com.bian.myapplication.activity.OpenGlActivity;
import com.bian.myapplication.activity.PlayAudioVideoActivity;
import com.bian.myapplication.activity.SurfaceViewActivity;
import com.bian.myapplication.dialog.SelectOptionDialog;
import com.bian.myapplication.image.ImageListActivity;
import com.bian.myapplication.utils.AppConstant;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.utils.NativePracise;
import com.bian.myapplication.utils.VideoUtil;
import com.bian.myapplication.activity.NewVideoPlayActivity;
import com.bian.myapplication.activity.PlayAudioActivity;
import com.bian.myapplication.activity.VideoPlayActivity;

import java.io.File;

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
    private Button toastButton, encodePictureBt;
    private CursorAdapter mCursorAdapter;
    private String[] videoProjects = new String[]{
            MediaStore.Video.Media._ID,
            MediaStore.Video.Media.DISPLAY_NAME,
            MediaStore.Video.Media.DATA};

    private SelectOptionDialog selectOptionDialog;
    private String mFilePath;
    private String saveVideodir;

    private Button testGLBt;
    private Button newButton, deleteButton;

    private Button surfaceBt;

    private NativePracise nativePracise;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
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
        nativePracise = new NativePracise();
        mListView = findViewById(R.id.lv_video);
        View headView = LayoutInflater.from(this).inflate(R.layout.headview_video_list, null);
        testGLBt = headView.findViewById(R.id.test_gl_button);
        newButton = headView.findViewById(R.id.test_new);
        deleteButton = headView.findViewById(R.id.test_delete);
        surfaceBt = headView.findViewById(R.id.bt_surface);
        surfaceBt.setOnClickListener(this);
        mListView.addHeaderView(headView);
        toastButton = findViewById(R.id.bt_toast);
        encodePictureBt = findViewById(R.id.bt_test_img);
        toastButton.setOnClickListener(this);
        encodePictureBt.setOnClickListener(this);
        testGLBt.setOnClickListener(this);
        newButton.setOnClickListener(this);
        deleteButton.setOnClickListener(this);
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
            case R.id.bt_toast:
                VideoUtil.testSoLibrary();
                break;
            case R.id.bt_test_img:
                Intent imgIt = new Intent(this, ImageActivity.class);
                startActivity(imgIt);
                break;
            case R.id.test_gl_button:
                startActivity(new Intent(this, OpenGlActivity.class));
                break;
            case R.id.test_new:
                nativePracise.newObject();
                break;
            case R.id.test_delete:
                nativePracise.deleteObject();
                break;
            case R.id.bt_surface:
                startActivity(new Intent(this, SurfaceViewActivity.class));
                break;

        }

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
        Cursor cursor = (Cursor) mCursorAdapter.getItem(position - 1);
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
                case 5:
                    usingCppPlayVideo();
                    break;
                case 6:
                    gotoAudioPlayActivity();
                case 7:
                    Intent audioPlayIt = new Intent(MainActivity.this, PlayAudioVideoActivity.class);
                    audioPlayIt.putExtra(AppConstant.ARG_VIDEO_PATH, mFilePath);
                    startActivity(audioPlayIt);
                    break;
            }
        }
    }

    private void gotoAudioPlayActivity() {
        Intent audioPlayIt = new Intent(this, PlayAudioActivity.class);
        audioPlayIt.putExtra(AppConstant.ARG_VIDEO_PATH, mFilePath);
        startActivity(audioPlayIt);
    }

    private void usingCppPlayVideo() {
        Intent cppPlayVideoit = new Intent(this, NewVideoPlayActivity.class);
        cppPlayVideoit.putExtra(AppConstant.ARG_VIDEO_PATH, mFilePath);
        startActivity(cppPlayVideoit);
    }


    private void playVideo() {
        Intent playVideoIt = new Intent(this, VideoPlayActivity.class);
        playVideoIt.putExtra(AppConstant.ARG_VIDEO_PATH, mFilePath);
        startActivity(playVideoIt);
    }

    private void addTextToVideo(String filePath, String markText) {
        String tempPath = saveVideodir + File.separator + "VideoInfo_" + System.currentTimeMillis() + ".avi";
        VideoUtil.addTextToVideo(filePath, markText, tempPath);
    }
}
