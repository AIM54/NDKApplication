//
// Created by Administrator on 2019/8/25.
//

#ifndef NDKAPPLICATION_MUTILTHREADPLAYER_H
#define NDKAPPLICATION_MUTILTHREADPLAYER_H


#include <jni.h>
#include <list>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

extern "C" {
#include <libavutil/frame.h>
#include <libavformat/avformat.h>
#include <GlobalConfig.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
};

class MutilThreadPlayer {
public:
    virtual void playVideo(JNIEnv *jniEnv, jobject surfaceView, char *videoUrl);

protected:
    AVFormatContext *avFormatContext;
    AVCodec *avCodec;
    AVCodecContext *avCodecContext;
    int videoIndex;

    virtual int openInputUrl(char *videoUrl);

    virtual int deocodeVideo(char *videoUrl);

    virtual int displayVideo(JNIEnv *jniEnv, jobject surfaceView);

    virtual void enqueueFrame(AVFrame *avFrame);

    AVFrame *popAvFrame();

    std::list<AVFrame *> avFrameList;
    std::atomic_bool isPlaying;
    std::mutex videoInforMutex;
    std::condition_variable videoInforCond;
    std::atomic_bool hasLoadVideoInfo{false};

    std::mutex videoQueueMutex;

    std::condition_variable produceCond;

    std::condition_variable consumerCond;

};


#endif //NDKAPPLICATION_MUTILTHREADPLAYER_H
