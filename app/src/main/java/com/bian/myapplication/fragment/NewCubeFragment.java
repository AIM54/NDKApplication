package com.bian.myapplication.fragment;


import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.fragment.app.Fragment;

import com.bian.myapplication.R;
import com.bian.myapplication.view.PictureGLSurfaceView;

/**
 */
public class NewCubeFragment extends Fragment {
    private PictureGLSurfaceView pictureGLSurfaceView;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_new_cube, container, false);
    }

}
