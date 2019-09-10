//
// Created by Administrator on 2019/9/10.
//

extern "C" {
#include "libavutil/frame.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
};

#include <android/native_window_jni.h>
#include "VideoDisplayer.h"

using namespace std;

int VideoDisplayer::pushIntoVideoList(AVFrame *avFrame) {
    unique_lock<mutex> videoListLock(videoListMutex);
    videoProducer.wait(videoListLock, [this] { return this->videoList.size() < 10; });
    videoList.push_back(avFrame);
    videoConsumer.notify_all();
    return 0;
}

int VideoDisplayer::playVideo() {
    isPlayingVideo= true;
    while (isPlayingVideo) {
        unique_lock<mutex> videoListLock(videoListMutex);
        videoConsumer.wait(videoListLock, [this] { return !this->videoList.empty(); });
        AVFrame *videoFrame = videoList.front();
        playVideoFrame(videoFrame);
        av_frame_free(&videoFrame);
        videoList.pop_front();
        videoProducer.notify_all();
    }
    return 0;
}

int VideoDisplayer::setForVideo(AVCodecContext *videoCodecContext) {
    return 0;
}