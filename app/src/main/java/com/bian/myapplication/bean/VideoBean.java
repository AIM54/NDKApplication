package com.bian.myapplication.bean;

public class VideoBean {
    public String fileName;
    public String filePath;

    @Override
    public String toString() {
        return "VideoBean{" +
                "fileName='" + fileName + '\'' +
                ", filePath='" + filePath + '\'' +
                '}';
    }
}
