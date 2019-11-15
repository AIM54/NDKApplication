//
// Created by Administrator on 2019/9/5.
//
extern "C" {
#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"
#include "libavutil/timestamp.h"
}


#include <GlobalConfig.h>
#include "AudioResampler.h"

AudioResampler::AudioResampler() {
    outputBuffer = new uint8_t[outputBufferSize];
}

int AudioResampler::initSwrContex(AVCodecContext *audioCodecContex) {
    swr_ctx = swr_alloc();
    if (!swr_ctx) {
        return -1;
    }
    /* set options */
    av_opt_set_int(swr_ctx, "in_channel_layout", audioCodecContex->channel_layout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", audioCodecContex->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", audioCodecContex->sample_fmt, 0);

    av_opt_set_int(swr_ctx, "out_channel_layout", dst_ch_layout, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", dst_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    if (swr_init(swr_ctx) < 0) {
        ALOGI("init swr failed");
        return -1;
    }
    return 0;
}

AudioFrameDataBean *
AudioResampler::getDataBean(AVFrame *audioFrame, AVCodecContext *audioCodecContext,
                            int audioStreamIndex,
                            AVFormatContext *audioFormatContext) {
    if (audioFrame->channels > 0 && audioFrame->channel_layout == 0) {
        audioFrame->channel_layout = av_get_default_channel_layout(
                audioFrame->channels);
    } else if (audioFrame->channels == 0 && audioFrame->channel_layout > 0) {
        audioFrame->channels = av_get_channel_layout_nb_channels(
                audioFrame->channel_layout);
    }
    int newDataSize = av_samples_get_buffer_size(audioFrame->linesize,
                                                 audioCodecContext->channels,
                                                 audioFrame->nb_samples,
                                                 audioCodecContext->sample_fmt,
                                                 1);
    dst_nb_samples = av_rescale_rnd(
            //得到输入sample和输出sample之间的延迟，并且其返回值的根据传入的第二个参数不同而不同。如果是输入的采样率，则返回值是输入sample个数；
            // 如果输入的是输出采样率，则返回值是输出sample个数。
            swr_get_delay(swr_ctx, audioCodecContext->sample_rate) +
            audioFrame->nb_samples,
            audioFrame->sample_rate, audioFrame->sample_rate, AV_ROUND_INF);

    if (newDataSize > outputBufferSize) {
        delete[] outputBuffer;
        outputBufferSize = newDataSize;
        outputBuffer = new uint8_t[newDataSize];
    }//在这里进行格式转换
    int nb = swr_convert(swr_ctx, &outputBuffer, dst_nb_samples,
                         (const uint8_t **) audioFrame->data, audioFrame->nb_samples);
    //根据布局获取声道数
    int out_channels = av_get_channel_layout_nb_channels(dst_ch_layout);
    int dataSize = out_channels * nb * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    AudioFrameDataBean *audioFrameDataBean = new AudioFrameDataBean(dataSize,
                                                                    outputBuffer);
    double displayTime = audioFrame->pts *
                         av_q2d(audioFormatContext->streams[audioStreamIndex]->time_base);
    ALOGI("av_ts2timestr:%s", av_ts2timestr(audioFrame->pts,
                                            &audioFormatContext->streams[audioStreamIndex]->time_base));
    audioFrameDataBean->setTimeStamp(displayTime);
    return audioFrameDataBean;
}

AudioResampler::~AudioResampler() {
    if (outputBuffer) {
        delete[]outputBuffer;
    }
    if (swr_ctx) {
        swr_free(&swr_ctx);
    }
}
