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
import com.bian.myapplication.utils.VideoUtil;

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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image_list);
        mListView = findViewById(R.id.lv_img);
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
        Cursor cursor = (Cursor) mCursorAdapter.getItem(position);
        String filePath = cursor.getString(cursor.getColumnIndex(MediaStore.Images.Media.DATA));
        Bitmap bitmap = BitmapFactory.decodeFile(filePath);
        BitMapCompressUtil.compressBitmap(bitmap,getCacheDir().getPath());

    }
}
