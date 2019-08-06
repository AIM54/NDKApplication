//
// Created by Administrator on 2019/7/12.
//

#include "NewPlayVideoInterface.h"
#include <set>
#include <android/window.h>
#include <android/native_window_jni.h>
#include <array>
#include <map>
#include <linux/media.h>


extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

using namespace std;

void NewPlayVideoInterface::openInput(string filePath) {
    av_register_all();
    AVFormatContext *avformat = avformat_alloc_context();
    if (!avformat) {
        return;
    }
    int resultCode = avformat_open_input(&avformat, filePath.data(), nullptr, nullptr);
    if (!resultCode) {
        return;
    }
    if (avformat_find_stream_info(avformat, NULL) < 0) {
        return;
    }

}

void NewPlayVideoInterface::playTheVideo() {
}

void NewPlayVideoInterface::pauseTheVideo() {
}

void NewPlayVideoInterface::seekToPosition(int position) {

}

void NewPlayVideoInterface::resumePlay() {

}

void NewPlayVideoInterface::release() {

}

