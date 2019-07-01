//
// Created by Administrator on 2019/6/28.
//
#include <jni.h>
#include <libavformat/avformat.h>

JNIEXPORT void JNICALL
Java_com_bian_myapplication_utils_VideoUtil_compressVideo(JNIEnv *env, jclass type,
                                                          jstring videoPath_, jstring targetPath_) {
    const char *videoPath = (*env)->GetStringUTFChars(env, videoPath_, 0);
    const char *targetPath = (*env)->GetStringUTFChars(env, targetPath_, 0);
    av_register_all();
    AVFormatContext *avFormatContext = avformat_alloc_context();
    AVFormatContext *outputAvFormatContext = avformat_alloc_context();
    AVOutputFormat *avOutputFormat = NULL;
    if (!avFormatContext) {
        return;
    }
    (*env)->ReleaseStringUTFChars(env, videoPath_, videoPath);
    (*env)->ReleaseStringUTFChars(env, targetPath_, targetPath);
}
