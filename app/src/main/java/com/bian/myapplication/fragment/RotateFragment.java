package com.bian.myapplication.fragment;


import android.os.Bundle;
import android.os.HandlerThread;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.fragment.app.Fragment;

import com.bian.myapplication.R;

/**
 * A simple {@link Fragment} subclass.
 */
public class RotateFragment extends Fragment {

    HandlerThread handlerThread=new HandlerThread("djfa",Thread.MAX_PRIORITY);
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        TextView textView = new TextView(getActivity());
        textView.setText(R.string.hello_blank_fragment);
        return textView;
    }

}
