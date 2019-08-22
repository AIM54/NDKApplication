//
// Created by Administrator on 2019/7/12.
//

#include "NewPlayVideoInterface.h"
#include <set>
#include <android/window.h>
#include <android/native_window_jni.h>
#include <array>
#include <linux/media.h>
#include<unistd.h>
#include <jni.h>
#include <thread>
#include "GlobalConfig.h"
#include <condition_variable>
#include<stdio.h>

using namespace std;
SLObjectItf playerObjItr;
SLPlayItf slPlayItf;
SLAndroidSimpleBufferQueueItf slAndroidSimpleBufferQueueItf;

SLEffectSendItf slEffectSendItf;

SLVolumeItf slVolumeItf;

atomic<int> audioChannels{0};

atomic<int> audioSample{0};

mutex audioStreamMutex;

condition_variable audioStreamInfoCondition;

std::mutex audioQueueMutex;

std::condition_variable produceCondition;

std::condition_variable consumerCondition;
static const SLEnvironmentalReverbSettings reverbSettings =
        SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

/**
 * 他妈的音频播放的回调队列
 * @param bq
 * @param context
 */
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {

}

NewPlayVideoInterface::NewPlayVideoInterface() : slEnvironmentalReverbSettings(
        SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR) {

}


void NewPlayVideoInterface::openInput(string filePath) {
    av_register_all();
    avformat = avformat_alloc_context();
    if (!avformat) {
        currentPlayState = PlayState::error;
        return;
    }
    int resultCode = avformat_open_input(&avformat, filePath.data(), nullptr, nullptr);
    if (resultCode < 0) {
        currentPlayState = PlayState::error;
        return;
    }
    if (avformat_find_stream_info(avformat, NULL) < 0) {
        currentPlayState = PlayState::error;
        return;
    }
}

void NewPlayVideoInterface::playAudio(std::string url) {
    thread decodeAudioThread(&NewPlayVideoInterface::decodeAudioMethod, this, url);
    decodeAudioThread.detach();
    playMusicInAndroid();
}


/**
 * 解析音频文件取出音频帧的代码
 * @param url
 * @return
 */
int NewPlayVideoInterface::decodeAudioMethod(std::string url) {
    unique_lock<mutex> decodeAudioLock(audioStreamMutex);
    FILE *ouputFile = fopen(outputAudioPath, "a+");
    if (!ouputFile) {
        ALOGI("openFile failed:%s", outputAudioPath);
    }
    openInput(url);
    if (currentPlayState == PlayState::error) {
        return -1;
    }
    uint8_t *outputBuffer;
    size_t outputBufferSize;
    if (decodeAudioData() <= 0) {
        return -1;
    }
    audioChannels.store(audioCodecContext->channels);
    audioSample.store(audioCodecContext->sample_rate);
    audioStreamInfoCondition.notify_all();
    decodeAudioLock.unlock();
    audioFrame = av_frame_alloc();
    audioPacket = av_packet_alloc();
    audioSwrContext = swr_alloc();
    av_opt_set_int(audioSwrContext, "in_channel_layout", audioCodecContext->channel_layout, 0);
    av_opt_set_int(audioSwrContext, "out_channel_layout", audioCodecContext->channel_layout, 0);
    av_opt_set_int(audioSwrContext, "in_sample_rate", audioCodecContext->sample_rate, 0);
    av_opt_set_int(audioSwrContext, "out_sample_rate", audioCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(audioSwrContext, "in_sample_fmt", audioCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(audioSwrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    ALOGI("the audio duration:", avformat->duration);
    swr_init(audioSwrContext);
    outputBufferSize = 8196;
    outputBuffer = new uint8_t[outputBufferSize];
    av_init_packet(audioPacket);
    int status = 0;
    while ((status = av_read_frame(avformat, audioPacket)) >= 0) {
        if (audioPacket->stream_index == audioStreamIndex) {
            status = avcodec_send_packet(audioCodecContext, audioPacket);
            if (status < 0) {
                ALOGI("failed to send audio packet\n");
                continue;
            }
            while (status >= 0) {
                status = avcodec_receive_frame(audioCodecContext, audioFrame);
                if (status == AVERROR(EAGAIN) || status == AVERROR_EOF) {
                    break;
                } else if (status < 0) {
                    ALOGI("Error during decoding3\n");
                    goto destrory;
                }
                int data_size = av_get_bytes_per_sample(audioCodecContext->sample_fmt);
                int newDataSize = av_samples_get_buffer_size(audioFrame->linesize,
                                                             audioCodecContext->channels,
                                                             audioFrame->nb_samples,
                                                             audioCodecContext->sample_fmt,
                                                             1);
                if (newDataSize > outputBufferSize) {
                    delete[] outputBuffer;
                    outputBufferSize = newDataSize;
                    outputBuffer = new uint8_t[newDataSize];
                }
                //在这里进行格式转换
                swr_convert(audioSwrContext, &outputBuffer, audioFrame->nb_samples,
                            const_cast<uint8_t const **>(audioFrame->extended_data),
                            audioFrame->nb_samples);
                AudioFrameDataBean *audioFrameDataBean = new AudioFrameDataBean(outputBufferSize,
                                                                                outputBuffer);
                pushToQueue(audioFrameDataBean);
                if (data_size < 0) {
                    /* This should not occur, checking just for paranoia */
                    ALOGI("Error during decoding4\n");
                    goto destrory;
                }
                double timeStamp =
                        audioFrame->pts * av_q2d(avformat->streams[audioStreamIndex]->time_base);
            }
        }
    }
    if (ouputFile) {
        fclose(ouputFile);
    }
    destrory:
    ALOGI("finish decode Audio frame");
    return -1;
}

void NewPlayVideoInterface::pushToQueue(AudioFrameDataBean *dataBean) {
    unique_lock<mutex> producerLock{audioQueueMutex};
    produceCondition.wait(producerLock, [this] { return isCanPushDataIntoAudioQueue(*this); });
    audioDeque.push_back(*dataBean);
    ALOGI("push the  frame into the queue:%d", audioDeque.size());
    consumerCondition.notify_all();
}


AudioFrameDataBean &NewPlayVideoInterface::popAudioFrameBean() {
    unique_lock<mutex> constumerLock{audioQueueMutex};
    consumerCondition.wait(constumerLock, [this] { return isAudioQueueNoEmpty(*this); });
    AudioFrameDataBean dataBean = audioDeque.front();
    audioDeque.pop_back();
    return dataBean;
}


bool isCanPushDataIntoAudioQueue(NewPlayVideoInterface &videoPlayer) {
    return videoPlayer.audioDeque.size() < 10;
}

bool isAudioQueueNoEmpty(NewPlayVideoInterface &videoPlayer) {
    return !videoPlayer.audioDeque.empty();
}

/**
 * 在这里进行Android音频播放器环境的创建
 * @return
 */
int NewPlayVideoInterface::playMusicInAndroid() {
    unique_lock<mutex> readLock(audioStreamMutex);
    audioStreamInfoCondition.wait(readLock, [] { return audioChannels.load() > 0; });
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

    ALOGI("the args of audio AudioChannels:%d  \n AudioSample:%d", audioChannels.load(),
          audioSample.load());

    //音频播放的缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue simpleBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    //音频播放的解析格式
    SLDataFormat_PCM slDataFormat_pcm={
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
                                                       bqPlayerCallback,
                                                       nullptr);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_EFFECTSEND, &slEffectSendItf);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_VOLUME, &slVolumeItf);
    ALOGI("the args of audio audioChannels:%d  \naudioSample： %d", audioChannels.load(),
          audioSample.load());

    return 0;
}

int NewPlayVideoInterface::decodeAudioData() {
    int status = -1;
    status = av_find_best_stream(avformat, AVMEDIA_TYPE_AUDIO, -1, -1, &audioCodec, 0);
    ALOGI("find Audio Stream");
    if (status < 0) {
        ALOGI("failed to find audio stream");
        return status;
    }
    audioStreamIndex = status;
    if (!audioCodec) {
        status = -1;
        ALOGI("failed to find audioCodec");
        return status;
    }
    audioCodecContext = avcodec_alloc_context3(audioCodec);
    if (!audioCodecContext) {
        status = -1;
        ALOGI("failed to find audioCodecContext");
        return status;
    }
    status = avcodec_parameters_to_context(audioCodecContext,
                                           avformat->streams[audioStreamIndex]->codecpar);
    if (status < 0) {
        return status;
    }
    if ((status = avcodec_open2(audioCodecContext, audioCodec, NULL)) < 0) {
        ALOGI("failed to open audio Stream ");
        return status;
    }
    return 1;
}

int NewPlayVideoInterface::decodeVideoData() {
    int status = -1;
    status = av_find_best_stream(avformat, AVMEDIA_TYPE_VIDEO, -1, -1, &videoCodec, 0);
    if (status < 0) {
        ALOGI("failed to find video stream");
        return status;
    }
    videoStreamIndex = status;
    if (!videoCodec) {
        status = -1;
        ALOGI("failed to find videoCodeer");
        return status;
    }
    videoCodecContext = avcodec_alloc_context3(videoCodec);
    if (!videoCodecContext) {
        status = -1;
        ALOGI("failed to find videoCodecContext");
        return status;
    }
    status = avcodec_parameters_to_context(videoCodecContext,
                                           avformat->streams[videoStreamIndex]->codecpar);
    if (status < 0) {
        return status;
    }
    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();
    av_init_packet(avPacket);
    if ((status = avcodec_open2(videoCodecContext, videoCodec, NULL)) < 0) {
        ALOGI("failed to open video Stream ");
        return status;
    }
    int readPacketStatus = 0;
    while ((readPacketStatus = av_read_frame(avformat, avPacket)) >= 0) {
        if (avPacket->stream_index == videoStreamIndex) {
            status = avcodec_send_packet(videoCodecContext, avPacket);
            if (status < 0) {
                ALOGI("Error sending a packet for decoding");
                break;
            }
            while (status >= 0) {
                status = avcodec_receive_frame(videoCodecContext, avFrame);
                if (status == AVERROR(EAGAIN) || status == AVERROR_EOF)
                    break;
                else if (status < 0) {
                    fprintf(stderr, "Error during decoding\n");
                    exit(1);
                }
                double timeStamp =
                        avFrame->pts * av_q2d(avformat->streams[videoStreamIndex]->time_base);
                if (timeStamp > 0) {
                    ALOGI("currentFrameTime: %f", timeStamp);
                }
            }
        }
    }
    ALOGI("readPacketStatus:%d", readPacketStatus);
    return 1;
}


void NewPlayVideoInterface::pauseTheVideo() {
}

void NewPlayVideoInterface::seekToPosition(int position) {

}

void NewPlayVideoInterface::resumePlay() {

}


NewPlayVideoInterface::~NewPlayVideoInterface() {
    if (!avFrame) {
        av_frame_free(&avFrame);
    }
    if (!avPacket) {
        av_packet_free(&avPacket);
    }
    if (!videoCodecContext) {
        avcodec_free_context(&videoCodecContext);
    }
    if (!audioCodecContext) {
        avcodec_free_context(&audioCodecContext);
    }
    if (!avformat) {
        avformat_free_context(avformat);
        avformat = nullptr;
    }
}


void NewPlayVideoInterface::playTheVideo() {
    int status = 0;
    if (currentPlayState == PlayState::error) {
        ALOGI("failed to open video File");
        return;
    }
    thread decodeVideoThread(&NewPlayVideoInterface::decodeVideoData, this);
    decodeVideoThread.detach();
}

void NewPlayVideoInterface::setAudioOutputPath(const char *string) {
    outputAudioPath = const_cast<char *>(string);
}



