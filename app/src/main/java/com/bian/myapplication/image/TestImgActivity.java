package com.bian.myapplication.image;

import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.bian.myapplication.R;

import androidx.appcompat.app.AppCompatActivity;

public class TestImgActivity extends AppCompatActivity implements AdapterView.OnItemClickListener {
    private ListView mListView;
    private ArrayAdapter<String> mAdapter;


    static {
        System.loadLibrary("NdkTest");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_img);
        mListView = findViewById(R.id.lv_main);
        mAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, getResources().getStringArray(R.array.test_img));
        mListView.setAdapter(mAdapter);
        mListView.setOnItemClickListener(this);
    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int position, long itemId) {
        switch (position) {
            case 0:
                testYuvImage();
                break;
        }
    }

    private native void testYuvImage();
}
