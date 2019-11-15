package com.bian.myapplication.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.bian.myapplication.R;
import com.bian.myapplication.view.PictureGLSurfaceView;
public class FirstFragment extends Fragment {
    private PictureGLSurfaceView pictureGLSurfaceView;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_first, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        pictureGLSurfaceView = view.findViewById(R.id.glv_main);
    }
    @Override
    public void onPause() {
        super.onPause();
        pictureGLSurfaceView.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
        pictureGLSurfaceView.onResume();
    }
}
