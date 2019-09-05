//
// Created by Administrator on 2019/9/5.
//

#ifndef NDKAPPLICATION_AUDIORESAMPLER_H
#define NDKAPPLICATION_AUDIORESAMPLER_H

#include <libavformat/avformat.h>
#include "AudioFrameDataBean.h"

extern "C" {
#include "libswresample/swresample.h"
#include "libavcodec/avcodec.h"
};


class AudioResampler {
private:
    /**
     * 音频转换上下文
     */
    SwrContext *swr_ctx;
    /**
     * 目标声道(暂时设定为双声道立体声)
     */
    int dst_ch_layout = AV_CH_LAYOUT_STEREO;
    /**
     * 频率
     */
    int dst_rate = 44100;

    size_t outputBufferSize = 8196;

    int dst_nb_samples;

    uint8_t *outputBuffer;
public:
    /**
     * 初始化转换器
     * @param avCodecContext  解码上下文
     * @return
     */
    int initSwrContex(AVCodecContext *avCodecContext);

    AudioFrameDataBean *
    getDataBean(AVFrame *avFrame, AVCodecContext *avCodecContext, int streamIndex,
                AVFormatContext *avFormatContext);

    AudioResampler();

    virtual ~AudioResampler();
};


#endif //NDKAPPLICATION_AUDIORESAMPLER_H
