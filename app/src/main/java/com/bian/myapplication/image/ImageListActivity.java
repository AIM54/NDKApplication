package com.bian.myapplication.image;

import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.SimpleCursorAdapter;

import com.bian.myapplication.R;
import com.bian.myapplication.utils.BitMapCompressUtil;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.loader.app.LoaderManager;
import androidx.loader.content.CursorLoader;
import androidx.loader.content.Loader;

public class ImageListActivity extends AppCompatActivity implements LoaderManager.LoaderCallbacks<Cursor>, AdapterView.OnItemClickListener {
    private ListView mListView;
    private String project[] = new String[]{
            MediaStore.Images.Media._ID,
            MediaStore.Images.Media.DATA,
            MediaStore.Images.Media.DISPLAY_NAME
    };
    private SimpleCursorAdapter mCursorAdapter;
    private ThreadPoolExecutor threadPoolExecutor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image_list);
        mListView = findViewById(R.id.lv_img);
        threadPoolExecutor = new ThreadPoolExecutor(Runtime.getRuntime().availableProcessors(), 10, 10, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>());
        initData();
    }

    private void initData() {
        LoaderManager manager = LoaderManager.getInstance(this);
        manager.initLoader(110, null, this);
    }


    @NonNull
    @Override
    public Loader<Cursor> onCreateLoader(int id, @Nullable Bundle args) {
        CursorLoader cursorLoader = new CursorLoader(this, MediaStore.Images.Media.EXTERNAL_CONTENT_URI, project, null, null, null);
        return cursorLoader;
    }

    @Override
    public void onLoadFinished(@NonNull Loader<Cursor> loader, Cursor data) {
        mCursorAdapter = new SimpleCursorAdapter(this,
                R.layout.i_video_info,
                data,
                new String[]{MediaStore.Images.Media.DISPLAY_NAME},
                new int[]{R.id.tv_name}, 0);
        mListView.setAdapter(mCursorAdapter);
        mListView.setOnItemClickListener(this);
    }

    @Override
    public void onLoaderReset(@NonNull Loader<Cursor> loader) {

    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        final Cursor cursor = (Cursor) mCursorAdapter.getItem(position);
        final String filePath = cursor.getString(cursor.getColumnIndex(MediaStore.Images.Media.DATA));
        threadPoolExecutor.execute(new Runnable() {
            @Override
            public void run() {
                Bitmap srcBitMap = BitmapFactory.decodeFile(filePath);
                BitMapCompressUtil.compressBitmap(getExternalCacheDir() + "234.jpg",srcBitMap);
            }
        });

    }
}
