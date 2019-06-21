package com.bian.myapplication;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CursorAdapter;
import android.widget.ListView;
import android.widget.SimpleCursorAdapter;
import android.widget.Toast;

import com.bian.myapplication.bean.VideoBean;
import com.bian.myapplication.utils.CommonLog;
import com.bian.myapplication.utils.VideoUtil;
import com.bian.myapplication.video.VideoPlayActivity;

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
    private Button toastButton;
    private CursorAdapter mCursorAdapter;
    private String[] videoProjects = new String[]{
            MediaStore.Video.Media._ID,
            MediaStore.Video.Media.DISPLAY_NAME,
            MediaStore.Video.Media.DATA};

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
            queryVideoInfo();
        }
    }

    private void queryVideoInfo() {
        LoaderManager manager = LoaderManager.getInstance(this);
        manager.initLoader(110, null, this);
    }

    private void initView() {
        mListView = findViewById(R.id.lv_video);
        toastButton = findViewById(R.id.bt_toast);
        toastButton.setOnClickListener(this);
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
        VideoUtil.testSoLibrary();
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
        String filePath = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.DATA));
        Intent videoPlayIt = new Intent(this, VideoPlayActivity.class);
        videoPlayIt.putExtra(VideoPlayActivity.ARG_VIDEO_PATH, filePath);
        startActivity(videoPlayIt);
    }
}
