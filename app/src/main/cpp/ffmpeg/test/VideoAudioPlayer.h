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

class VideoAudioPlayer {
public:
    /**
     * 播放视频文件
     * @param videourl  文件目录
     * @param surfaceView  surfaceView
     * @return
     */
    virtual int playVideo(char *videourl, jobject surfaceView);

protected:

    virtual int openIntput(char *videoUrl);

    /**
     * 音频队列同步锁
     */
    std::mutex audioListMutex;
    /**
     * 音频数据缓冲队列
     */
    std::list<AudioFrameDataBean> audioDataList;
    /**
     * 音频数据转化为pcm数据后的播放器
     */
    AndroidPcmPlayer *androidPcmPlayer;

    /**
     * 获取音频数据
     * @return
     */
    virtual AudioFrameDataBean &getAudioFrameBean();

    /**
     * 音频数据重采样,
     */
    AudioResampler *audioResampler;

    /**
     * 音频解码器
     */
    AVCodec *audioCodec;
    /**
     * 音频解码上下文
     */
    AVCodecContext *audioCodecContex;

    /**
     * 判断是否在继续播放
     */
    std::atomic_bool isPlaying{false};


    /**
     * 打开解码器
     * @param streamIndex 媒体流的下标
     * @param avCodec
     * @param avCodecContext
     * @return
     */
    virtual int initCodec(int &streamIndex, AVCodec *avCodec, AVCodecContext *avCodecContext);


    std::list<AVFrame *> videoFrameList;

    virtual int decodeStream();

    virtual int decodePacket(AVCodecContext *avCodecContext, AVPacket *avPacket, AVFrame *avFrame);

    virtual int decodeVideoFrame(AVFrame *avFrame);

    virtual int decodeAudioFrame(AVFrame *audioFrame);

    virtual int displayScreen(jobject surface);

    virtual int initSwrContext();

    virtual int playSound();


    AVFormatContext *avFormatContext;

    int videoIndex;

    int audioIndex;

    AVCodec *videoCodec;
    AVCodecContext *videoCodecContext;




};


#endif //NDKAPPLICATION_VIDEOAUDIOPLAYER_H
