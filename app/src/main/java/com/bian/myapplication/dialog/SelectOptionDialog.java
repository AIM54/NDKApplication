package com.bian.myapplication.dialog;

import android.app.Dialog;
import android.content.Context;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.bian.myapplication.R;

import androidx.annotation.NonNull;

public class SelectOptionDialog extends Dialog  {
    private final String[] videoOpertions;

    private ListView mListView;

    private ArrayAdapter<String> optionAdapter;

    private AdapterView.OnItemClickListener onItemClickListener;

    public SelectOptionDialog(@NonNull Context context, AdapterView.OnItemClickListener itemClickListener) {
        this(context, R.style.CustomDialog);
        this.onItemClickListener=itemClickListener;
        mListView.setOnItemClickListener(itemClickListener);
    }

    public SelectOptionDialog(@NonNull Context context, int themeResId) {
        super(context, themeResId);
        setContentView(R.layout.d_list_view);
        mListView = findViewById(R.id.lv_option);
        videoOpertions = getContext().getResources().getStringArray(R.array.video_opertion_item);
        optionAdapter = new ArrayAdapter<String>(getContext(), android.R.layout.simple_list_item_1, videoOpertions);
        mListView.setAdapter(optionAdapter);
        if (onItemClickListener!=null){
            mListView.setOnItemClickListener(onItemClickListener);
        }
    }

    public void setOptionAdapter(ArrayAdapter<String> optionAdapter) {
        this.optionAdapter = optionAdapter;
        mListView.setAdapter(optionAdapter);
    }
}
