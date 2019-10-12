//
// Created by Administrator on 2019/9/5.
//

#ifndef NDKAPPLICATION_VIDEOAUDIOPLAYER_H
#define NDKAPPLICATION_VIDEOAUDIOPLAYER_H


#include <jni.h>

extern "C" {
#include "libavutil/frame.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
};

#include "Runnable.h"
#include <list>
#include <AudioFrameDataBean.h>
#include <condition_variable>
#include "AndroidPcmPlayer.h"
#include "AudioResampler.h"
#include "VideoDisplayer.h"
#include "AudioDecoder.h"
#include "VideoDecoder.h"
#include "Thread.h"


class VideoAudioPlayer : public Runnable {
private:
    JNIEnv *pEnv;
    char *videoUrl;
    ANativeWindow *aNativeWindow;
    Thread *decodeThread;
public:
    /**
     * 播放视频文件
     * @param videourl  文件目录
     * @param surfaceView  surfaceView
     * @return
     */
    virtual int playVideo(char *videourl, JNIEnv *pEnv, jobject surfaceView);

    virtual int stopPlay();

    virtual ~VideoAudioPlayer();

protected:
    /**
     * 音频数据转化为pcm数据后的播放器
     */
    AndroidPcmPlayer *androidPcmPlayer = nullptr;

    /**
     * 音频数据重采样,
     */
    AudioResampler *audioResampler = nullptr;

    /**
     * 判断是否在继续播放
     */
    std::atomic_bool isPlaying{false};

    AudioDecoder *audioDecoder = nullptr;
    VideoDecoder *videoDecoder = nullptr;

    virtual int readStreamInfor();

    virtual int openIntput(char *videoUrl);

    virtual int initDecoder(int streamIndex);

    AVFormatContext *avFormatContext = nullptr;

    int videoIndex;

    int audioIndex;

    void runningTask() override;
};


#endif //NDKAPPLICATION_VIDEOAUDIOPLAYER_H
