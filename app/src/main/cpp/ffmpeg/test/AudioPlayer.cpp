//
// Created by Administrator on 2019/8/31.
//

#include "AudioPlayer.h"
#include "GlobalConfig.h"
#include <thread>
#include <SLES/OpenSLES.h>
#include <atomic>
#include <SLES/OpenSLES_Android.h>
#include "AudioFrameDataBean.h"
/**
 * 对于packed格式的数据（例如RGB24），会存到data[0]里面。
对于planar格式的数据（例如YUV420P），则会分开成data[0]，data[1]，data[2]...（YUV420P中data[0]存Y，data[1]存U，data[2]存V）
 因为opensl es 的缘故我们把所有的音频数据转化为packed 格式，采样的话，使用一个一维数组就可以了

 每秒理论PCM大小 = 采样率 * 声道数 * 位数/8
 如:
 int s_time = 44100 * 2 * 16/8;
 */
extern "C" {
#include "libavcodec/avcodec.h";
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/timestamp.h"
}
static AVFormatContext *audioFormatContext;
static AVCodec *audioCodec = nullptr;
static AVCodecContext *audioCodecContext;
static AVCodecParameters *audioAvCodecParameters;
static int audioStreamIndex;

static int dst_ch_layout = AV_CH_LAYOUT_STEREO;
static int dst_rate = 44100;
static int dst_nb_samples;
static SLObjectItf playerObjItr;
static SLPlayItf slPlayItf;
static SLAndroidSimpleBufferQueueItf slAndroidSimpleBufferQueueItf;

static SLEffectSendItf slEffectSendItf;

static SLVolumeItf slVolumeItf;
static std::condition_variable produceCondition;

static std::condition_variable consumerCondition;

static std::mutex audioListMutex;

static const SLEnvironmentalReverbSettings reverbSettings =
        SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

static std::atomic<AudioPlayer *> thisAudioPlayer{nullptr};
SLObjectItf slObjectItf1 = nullptr;
SLEngineItf slEngine;
SLObjectItf outputMixObject;
SLEnvironmentalReverbItf outputMixoutputEnvironmentalReverbIter = nullptr;


using namespace std;

int AudioPlayer::decodeAudioData(char *url) {
    int status = openInputUrl(url);

    if (status < 0) {
        ALOGI("failed to openFile");
        return status;
    }
    AVPacket *audioPacket = av_packet_alloc();
    if (!audioPacket) {
        return -1;
    }
    SwrContext *swr_ctx = swr_alloc();
    if (!swr_ctx) {
        return -1;
    }
    /* set options */
    av_opt_set_int(swr_ctx, "in_channel_layout", audioCodecContext->channel_layout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", audioCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", audioCodecContext->sample_fmt, 0);

    av_opt_set_int(swr_ctx, "out_channel_layout", dst_ch_layout, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", dst_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    if ((status = swr_init(swr_ctx)) < 0) {
        ALOGE("failed to init swrContext12:%d", status);
        return -1;
    }
    av_init_packet(audioPacket);
    AVFrame *audioFrame = av_frame_alloc();
    if (!audioFrame) {
        ALOGI("failed to alloc frame");
        return -1;
    }
    //音频帧的数据量计算
    // 一帧音频的数据量=channel数 * nb_samples样本数 * 每个样本占用的字节数
    //ffmpeg 每一帧的默认采用数就是1024，
    //每秒大概会播放43帧音频数据
    //如果该音频帧是FLTP格式的PCM数据，包含1024个样本，双声道，那么该音频帧包含的音频数据量是2*1024*4=8192字节
    size_t outputBufferSize = 8192;
    uint8_t *outputBuffer = new uint8_t[outputBufferSize];
    while ((status = av_read_frame(audioFormatContext, audioPacket)) >= 0) {
        if (audioPacket->stream_index == audioStreamIndex) {
            status = avcodec_send_packet(audioCodecContext, audioPacket);
            if (status < 0) {
                ALOGI("faild to send_packet");
                av_packet_unref(audioPacket);
                break;
            }
            while (status >= 0) {
                status = avcodec_receive_frame(audioCodecContext, audioFrame);
                if (status == AVERROR(EAGAIN) || status == AVERROR_EOF) {
                    av_frame_unref(audioFrame);
                    continue;
                } else if (status < 0) {
                    ALOGI("faild to receive frame2");
                    av_frame_free(&audioFrame);
                    av_packet_free(&audioPacket);
                    break;
                }
                //设置通道数量和channel layout
                //在编码的时候有可能丢失通道数量或者channel layout ，这里根据获取的参数设置其默认值
                if (audioFrame->channels > 0 && audioFrame->channel_layout == 0) {
                    audioFrame->channel_layout = av_get_default_channel_layout(
                            audioFrame->channels);
                } else if (audioFrame->channels == 0 && audioFrame->channel_layout > 0) {
                    audioFrame->channels = av_get_channel_layout_nb_channels(
                            audioFrame->channel_layout);
                }
                //获取每一帧的大小
                int newDataSize = av_samples_get_buffer_size(audioFrame->linesize,
                                                             audioCodecContext->channels,
                                                             audioFrame->nb_samples,
                                                             audioCodecContext->sample_fmt,
                                                             1);
                dst_nb_samples = av_rescale_rnd(
                        //得到输入sample和输出sample之间的延迟，并且其返回值的根据传入的第二个参数不同而不同。如果是输入的采样率，则返回值是输入sample个数
                        // 如果输入的是输出采样率，则返回值是输出sample个数。
                        swr_get_delay(swr_ctx, audioCodecContext->sample_rate) +
                        audioFrame->nb_samples,
                        audioFrame->sample_rate, audioFrame->sample_rate, AV_ROUND_INF);

                if (newDataSize > outputBufferSize) {
                    delete[] outputBuffer;
                    outputBufferSize = newDataSize;
                    outputBuffer = new uint8_t[newDataSize];
                }
                //在这里进行格式转换,返回是每个声道的采样数
                int outSampleNumber = swr_convert(swr_ctx, &outputBuffer, dst_nb_samples,
                                                  (const uint8_t **) audioFrame->data,
                                                  audioFrame->nb_samples);
                //根据布局获取声道数
                int out_channels = av_get_channel_layout_nb_channels(dst_ch_layout);
                //这里是为了获取采集到的一帧音频数据的大小，采集到的数据大小==声道数*采样数*采样深度
                //因为我们这里播放音频的时候使用的是packed 格式的音频而不是planner
                int dataSize =
                        out_channels * outSampleNumber * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

                AudioFrameDataBean *audioFrameDataBean = new AudioFrameDataBean(dataSize,
                                                                                outputBuffer);
                double displayTime = audioFrame->pts *
                                     av_q2d(audioFormatContext->streams[audioStreamIndex]->time_base);
                ALOGI("av_ts2timestr:%s", av_ts2timestr(audioFrame->pts,
                                                        &audioFormatContext->streams[audioStreamIndex]->time_base));
                audioFrameDataBean->setTimeStamp(displayTime);
                if (dataSize > 0) {
                    pushAudioFrameIntoList(audioFrameDataBean);
                }
                delete audioFrameDataBean;
                av_frame_unref(audioFrame);
            }
        }
        av_packet_unref(audioPacket);
    }
    delete[]outputBuffer;
    return 0;
}


int AudioPlayer::openInputUrl(char *url) {
    av_register_all();
    audioFormatContext = avformat_alloc_context();
    if (!audioFormatContext) {
        ALOGI("failed to alloc formatContx");
        return -1;
    }
    int status = avformat_open_input(&audioFormatContext, url, nullptr, nullptr);
    if (status < 0) {
        ALOGI("failed to open the file url:%s\n the failed Code :%d", url, status);
        return -1;
    }
    if ((status = avformat_find_stream_info(audioFormatContext, nullptr)) < 0) {
        ALOGI("failed to find stream infor");
        return status;
    }

    for (int videoStreamIndex = 0;
         videoStreamIndex < audioFormatContext->nb_streams; ++videoStreamIndex) {
        if (audioFormatContext->streams[videoStreamIndex]->codecpar->codec_type ==
            AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = videoStreamIndex;
            break;
        }
    }
    audioCodec = avcodec_find_decoder(
            audioFormatContext->streams[audioStreamIndex]->codecpar->codec_id);
    if (!audioCodec) {
        ALOGI("failed to find audioCodec");
        return -1;
    }
    audioCodecContext = avcodec_alloc_context3(audioCodec);
    if (!audioCodecContext) {
        ALOGI("failed to alloc codecContext");
        return -1;
    }
    audioAvCodecParameters = avcodec_parameters_alloc();
    if (!audioAvCodecParameters) {
        ALOGI("failed to alloc codecParameters");
        return -1;
    }
    status = avcodec_parameters_to_context(audioCodecContext,
                                           audioFormatContext->streams[audioStreamIndex]->codecpar);
    if (status < 0) {
        ALOGI("failed to init codecParameters");
        return status;
    }
    if ((status = avcodec_open2(audioCodecContext, audioCodec, nullptr)) < 0) {
        ALOGI("failed to  avcodec_open2");
        return status;
    }
    return status;
}

int AudioPlayer::playAudio(char *url) {
    thisAudioPlayer.store(this);
    thread decodeAudioThread(&AudioPlayer::decodeAudioData, this, url);
    decodeAudioThread.detach();
    thread displayMusicThread(&AudioPlayer::playAudioAndroid, this);
    displayMusicThread.detach();
    return 0;
}

int AudioPlayer::playAudioAndroid() {
    SLresult sLresult;
    sLresult = slCreateEngine(&slObjectItf1, 0, NULL, 0, NULL, NULL);
    sLresult = (*slObjectItf1)->Realize(slObjectItf1, SL_BOOLEAN_FALSE);
    sLresult = (*slObjectItf1)->GetInterface(slObjectItf1, SL_IID_ENGINE, &slEngine);

    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    sLresult = (*slEngine)->CreateOutputMix(slEngine, &outputMixObject, 1, ids, req);
    sLresult = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    sLresult = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                                &outputMixoutputEnvironmentalReverbIter);
    if (sLresult == SL_RESULT_SUCCESS) {
        sLresult = (*outputMixoutputEnvironmentalReverbIter)->SetEnvironmentalReverbProperties(
                outputMixoutputEnvironmentalReverbIter, &reverbSettings);
    }
    //音频播放的缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue simpleBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    //音频播放的解析格式,,这里有bug不知道为什么不能使用上面的参数
    SLDataFormat_PCM slDataFormat_pcm = {
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            2,//2个声道（立体声）
            SL_SAMPLINGRATE_44_1,//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };
    SLDataSource slDataSource{&simpleBufferQueue, &slDataFormat_pcm};
    SLDataLocator_OutputMix slDataLocator_outputMix{SL_DATALOCATOR_OUTPUTMIX, outputMixObject};

    SLDataSink slDataSink{&slDataLocator_outputMix, NULL};

    const SLInterfaceID playerIds[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/};
    const SLboolean playReq[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ };
    if (sLresult != SL_RESULT_SUCCESS) {
        ALOGI("232 the reson is :%d", sLresult);
        return -1;
    }

    sLresult = (*slEngine)->CreateAudioPlayer(slEngine, &playerObjItr, &slDataSource, &slDataSink,
                                              3,
                                              playerIds, playReq);
    if (sLresult != SL_RESULT_SUCCESS) {
        ALOGI("CreateAudioFailed the reson is :%d", sLresult);
        return -1;
    }

    (*playerObjItr)->Realize(playerObjItr, SL_BOOLEAN_FALSE);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_PLAY, &slPlayItf);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_BUFFERQUEUE, &slAndroidSimpleBufferQueueItf);

    (*slAndroidSimpleBufferQueueItf)->RegisterCallback(slAndroidSimpleBufferQueueItf,
                                                       bqNewPlayerCallback,
                                                       nullptr);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_EFFECTSEND, &slEffectSendItf);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_VOLUME, &slVolumeItf);
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
    bqNewPlayerCallback(slAndroidSimpleBufferQueueItf, NULL);
    return 0;
}

void AudioPlayer::pushAudioFrameIntoList(AudioFrameDataBean *audioFrameDataBean) {
    unique_lock<mutex> audioLock(audioListMutex);
    produceCondition.wait(audioLock, [this] { return this->audioFrameList.size() < 1; });
    audioFrameList.push_back(*audioFrameDataBean);
    consumerCondition.notify_all();
}

void bqNewPlayerCallback(SLAndroidSimpleBufferQueueItf audioPlayQueue, void *context) {
    AudioPlayer *audioPlayer = thisAudioPlayer.load();
    unique_lock<mutex> audioLock(audioListMutex);
    consumerCondition.wait(audioLock,
                           [audioPlayer] { return !audioPlayer->audioFrameList.empty(); });
    AudioFrameDataBean audioFrameDataBean = audioPlayer->audioFrameList.front();
    (*audioPlayQueue)->Enqueue(audioPlayQueue, audioFrameDataBean.getData(),
                               audioFrameDataBean.getSize());
    ALOGI("the displayTime in ConsumerThread:%f", audioFrameDataBean.getTimeStamp());
    audioPlayer->audioFrameList.pop_front();
    produceCondition.notify_all();
}




