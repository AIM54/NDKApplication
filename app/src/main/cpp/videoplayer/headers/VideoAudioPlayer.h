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

#include <list>
#include <AudioFrameDataBean.h>
#include <condition_variable>
#include "AndroidPcmPlayer.h"
#include "AudioResampler.h"
#include "VideoDisplayer.h"


class VideoAudioPlayer {
private:
    JNIEnv *pEnv;
public:
    /**
     * 播放视频文件
     * @param videourl  文件目录
     * @param surfaceView  surfaceView
     * @return
     */
    virtual int playVideo(char *videourl, JNIEnv *pEnv, jobject surfaceView);

protected:

    virtual int openIntput(char *videoUrl);

    virtual int decodeAudio(char *videoUrl);

    virtual int decodeVideo(char *videoUrl);

    /**
     * 音频数据转化为pcm数据后的播放器
     */
    AndroidPcmPlayer *androidPcmPlayer = nullptr;


    /**
     * 音频数据重采样,
     */
    AudioResampler *audioResampler = nullptr;

    /**
     * 音频解码器
     */
    AVCodec *audioCodec = nullptr;
    /**
     * 音频解码上下文
     */
    AVCodecContext *audioCodecContex = nullptr;

    /**
     * 判断是否在继续播放
     */
    std::atomic_bool isPlaying{false};

    /**
     * 初始化音频解码器
     * @return
     */
    virtual int initSwrContext();

    /**
     * 播放声音
     * @return 
     */
    virtual int playSound();

    /**
     * 音频信息的等待条件
     */
    std::condition_variable audioInforCond;

    /**
     * 音频信息的锁对象
     */
    std::mutex audioInforMutex;

    std::atomic_bool isGetAudioInfor{false};

    std::atomic_bool isGetStreamInfor{false};

    std::mutex streamInforMutex;
    /**
     * 视频文件信息条件
     */
    std::condition_variable streamInforCond;

    VideoDisplayer *videoDisplayer;

    /**
     * 打开解码器
     * @param streamIndex 媒体流的下标
     * @param avCodec
     * @param avCodecContext
     * @return
     */
    virtual int initCodec(int &streamIndex, AVCodec **avCodec, AVCodecContext **avCodecContext);


    /**
     * 从视频流当中读取出包
     * @return 
     */
    virtual int decodeStream(int index, AVCodecContext *avCodecContext);


    /**
     * 从包中读取帧信息
     * @param avCodecContext 
     * @param avPacket 
     * @param avFrame 
     * @return 
     */
    virtual int decodePacket(AVCodecContext *avCodecContext, AVPacket *avPacket, AVFrame *avFrame);

    virtual int decodeVideoFrame(AVFrame *avFrame);

    virtual int decodeAudioFrame(AVFrame *audioFrame);


    virtual int displayVideo(jobject surface);

    AVFormatContext *avFormatContext;

    int videoIndex;

    int audioIndex;

    AVCodec *videoCodec = nullptr;
    AVCodecContext *videoCodecContext = nullptr;
};


#endif //NDKAPPLICATION_VIDEOAUDIOPLAYER_H
