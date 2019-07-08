//
// Created by Administrator on 2019/7/2.
//
#include <jni.h>
#include <libavformat/avformat.h>
#include <stdio.h>
#include <GlobalConfig.h>


#define INBUF_SIZE 4096


void JNICALL
Java_com_bian_myapplication_utils_VideoUtil_decodeImage(JNIEnv *env, jclass type,
                                                        jstring filePath_) {
    av_register_all();
    avformat_network_init();
    AVFormatContext *avFormatContext = avformat_alloc_context();
    if (!avFormatContext) {
        return;
    }
     avio_open()
}
