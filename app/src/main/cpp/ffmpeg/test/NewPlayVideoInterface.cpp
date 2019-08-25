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
#include <android/window.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

using namespace std;
SLObjectItf playerObjItr;
SLPlayItf slPlayItf;
SLAndroidSimpleBufferQueueItf slAndroidSimpleBufferQueueItf;

SLEffectSendItf slEffectSendItf;

SLVolumeItf slVolumeItf;

atomic<int> audioChannels{0};

atomic<int> audioSample{0};

atomic<bool> isPlayingVideoStream{false};


/**
 * 音频信息同步条件
 */
condition_variable audioStreamInfoCondition;
/**
 * 音频队列同步锁
 */
std::mutex audioQueueMutex;

/**
 * 音频信息同步锁
 */
mutex audioStreamMutex;
atomic_bool hasLoadVideoInfor{false};
mutex videoInforMutex;
condition_variable videoInfoCondition;

mutex videoQueueMutex;
condition_variable videoProducerCondition;
condition_variable videoConsumerCondition;


std::condition_variable produceCondition;

std::condition_variable consumerCondition;

static const SLEnvironmentalReverbSettings reverbSettings =
        SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

atomic<NewPlayVideoInterface *> thisPlayer{nullptr};


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
    thisPlayer.store(this);
    thread decodeAudioThread(&NewPlayVideoInterface::decodeAudioMethod, this, url);
    decodeAudioThread.detach();
    thread playAndroidMusicThread(&NewPlayVideoInterface::playMusicInAndroid, this);
    playAndroidMusicThread.detach();
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
                if (isQuiet.load()) {
                    goto destrory;
                }
                status = avcodec_receive_frame(audioCodecContext, audioFrame);
                if (status == AVERROR(EAGAIN) || status == AVERROR_EOF) {
                    break;
                } else if (status < 0) {
                    ALOGI("Error during decoding3\n");
                    goto destrory;
                }
                int newDataSize = av_samples_get_buffer_size(audioFrame->linesize,
                                                             audioCodecContext->channels,
                                                             audioFrame->nb_samples,
                                                             audioCodecContext->sample_fmt,
                                                             1);

                ALOGI("the new Data Size:%d", newDataSize);
                if (newDataSize > outputBufferSize) {
                    delete[] outputBuffer;
                    outputBufferSize = newDataSize;
                    outputBuffer = new uint8_t[newDataSize];
                }
                //在这里进行格式转换
                int length = swr_convert(audioSwrContext, &outputBuffer, audioFrame->nb_samples,
                                         const_cast<uint8_t const **>(audioFrame->extended_data),
                                         audioFrame->nb_samples);
                AudioFrameDataBean audioFrameDataBean(outputBufferSize,
                                                      outputBuffer);
                pushToQueue(audioFrameDataBean);
                if (newDataSize < 0) {
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
    if (!audioSwrContext) {
        swr_free(&audioSwrContext);
    }
    if (!audioFrame) {
        av_frame_free(&audioFrame);
    }
    if (!audioPacket) {
        av_packet_unref(audioPacket);
        av_packet_free(&audioPacket);
    }
    if (!outputBuffer) {
        delete[] outputBuffer;
    }
    if (!audioCodecContext) {
        avcodec_free_context(&audioCodecContext);
    }
    if (!avformat) {
        avformat_close_input(&avformat);
    }
    return -1;
}

void NewPlayVideoInterface::pushToQueue(AudioFrameDataBean &dataBean) {
    unique_lock<mutex> producerLock{audioQueueMutex};
    produceCondition.wait(producerLock, [this] { return isCanPushDataIntoAudioQueue(*this); });
    audioDeque.push_back(dataBean);
    ALOGI("push the  frame into the queue:%d", audioDeque.size());
    consumerCondition.notify_all();
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
    unique_lock<mutex> readLock(audioStreamMutex);
    audioStreamInfoCondition.wait(readLock, [] { return audioChannels.load() > 0; });
    SLuint32 channels = audioChannels.load();
    SLuint32 sample = audioSample.load();
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
                                                       bqPlayerCallback,
                                                       nullptr);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_EFFECTSEND, &slEffectSendItf);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_VOLUME, &slVolumeItf);
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
    ALOGI("the args of audio audioChannels:%d  \naudioSample： %d", audioChannels.load(),
          audioSample.load());
    bqPlayerCallback(slAndroidSimpleBufferQueueItf, NULL);
    return 0;
}

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf audioPlayQueue, void *context) {
    if (!thisPlayer.load()) {
        exit(0);
    }
    NewPlayVideoInterface *videoInterface = thisPlayer.load();
    unique_lock<mutex> consumerLock(audioQueueMutex);
    consumerCondition.wait(consumerLock,
                           [videoInterface] { return isAudioQueueNoEmpty(*videoInterface); });
    AudioFrameDataBean audioFrameDataBean = videoInterface->audioDeque.front();
    (*audioPlayQueue)->Enqueue(audioPlayQueue, audioFrameDataBean.getData(),
                               audioFrameDataBean.getSize());
    videoInterface->audioDeque.pop_front();
    produceCondition.notify_all();
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
    int videoIndex = -1;
    for (int index = 0; index < avformat->nb_streams; ++index) {
        if (avformat->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = index;
            break;
        }
    }
    if (videoIndex < 0) {
        ALOGI("failed to find video stream");
        return status;
    }
    videoStreamIndex = videoIndex;

    double videoLength = avformat->streams[videoStreamIndex]->duration *
                         av_q2d(avformat->streams[videoStreamIndex]->time_base);
    ALOGI("the totalLength of Video is: %f", videoLength);
    AVCodec *videoCodec = avcodec_find_decoder(
            avformat->streams[videoStreamIndex]->codecpar->codec_id);
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
    hasLoadVideoInfor.store(true);
    videoInfoCondition.notify_all();
    int readPacketStatus;
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
                AVFrame *videoFrame = av_frame_alloc();
                if (av_frame_ref(videoFrame, avFrame) >= 0) {
                    pushVideoFrameToQueue(videoFrame);
                }
                av_frame_unref(avFrame);
            }
        }
    }
    return 1;
}


void NewPlayVideoInterface::pushVideoFrameToQueue(AVFrame *avFrame) {
    unique_lock<mutex> videoQueueLock(videoQueueMutex);
    videoProducerCondition.wait(videoQueueLock,
                                [this] { return this->videoFrameList.size() < 10; });
    videoFrameList.push_back(avFrame);
    videoConsumerCondition.notify_all();
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


void NewPlayVideoInterface::playTheVideo(JNIEnv *pEnv, jobject pJobject, JavaVM *pVM) {
    int status = 0;
    if (currentPlayState == PlayState::error) {
        ALOGI("failed to open video File");
        return;
    }
    if (!pVM) {
        ALOGI("get JavaVm failed");
        return;
    }
    isPlayingVideoStream.store(true);
    thread decodeVideoThread(&NewPlayVideoInterface::decodeVideoData, this);
    decodeVideoThread.detach();
    showVideoFrame(pEnv, pJobject, pVM);
}

void NewPlayVideoInterface::showVideoFrame(JNIEnv *pEnv, jobject surfaceHolder, JavaVM *javaVM) {
    ANativeWindow *videoWindow = ANativeWindow_fromSurface(pEnv, surfaceHolder);
    unique_lock<mutex> videoInfoLock(videoInforMutex);
    ALOGI("before wait for videoCodecContext");
    videoInfoCondition.wait(videoInfoLock, [this] {
        return hasLoadVideoInfor.load();
    });
    ALOGI("after wait for videoCodecContext.width:%d||height:%d", videoCodecContext->width,
          videoCodecContext->height);
    ANativeWindow_setBuffersGeometry(videoWindow, videoCodecContext->width,
                                     videoCodecContext->height, WINDOW_FORMAT_RGBA_8888);
    int imageBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, videoCodecContext->width,
                                                   videoCodecContext->height,
                                                   1);
    uint8_t *bufferArray = static_cast<uint8_t *>(av_malloc(imageBufferSize * sizeof(uint8_t)));
    AVFrame *rgbFrame = av_frame_alloc();
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, bufferArray, AV_PIX_FMT_RGBA,
                         videoCodecContext->width, videoCodecContext->height, 0);

    struct SwsContenxt *swsContenxt = reinterpret_cast<struct SwsContenxt *>(sws_getContext(
            videoCodecContext->width,
            videoCodecContext->height,
            videoCodecContext->pix_fmt,
            videoCodecContext->width, videoCodecContext->height,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR, NULL, NULL, NULL));
    while (isPlayingVideoStream.load()) {
        unique_lock<mutex> videoQueueLock(videoQueueMutex);
        videoConsumerCondition.wait(videoQueueLock,
                                    [this] { return !this->videoFrameList.empty(); });
        AVFrame *srcFrame = videoFrameList.front();
        ANativeWindow_Buffer windowBuffer;
        ANativeWindow_lock(videoWindow, &windowBuffer, 0);
        sws_scale(reinterpret_cast<SwsContext *>(swsContenxt),
                  (uint8_t const *const *) srcFrame->data, srcFrame->linesize, 0,
                  videoCodecContext->height,
                  rgbFrame->data, rgbFrame->linesize);
        uint8_t *dstBits = static_cast<uint8_t *>(windowBuffer.bits);
        int dstStride = windowBuffer.stride * 4;
        uint8_t *src = rgbFrame->data[0];
        int srcStride = rgbFrame->linesize[0];
        for (int i = 0; i < videoCodecContext->height; ++i) {
            memcpy(dstBits + i * dstStride, src + i * srcStride, srcStride);
        }
        double timeStamp =srcFrame->pts * av_q2d(avformat->streams[videoStreamIndex]->time_base);
        if (timeStamp > 0) {
            ALOGI("currentFrameTime: %f", timeStamp);
        }
        ANativeWindow_unlockAndPost(videoWindow);
        videoFrameList.pop_front();
        videoProducerCondition.notify_all();
    }
}


void NewPlayVideoInterface::setAudioOutputPath(const char *string) {
    outputAudioPath = const_cast<char *>(string);
}

void NewPlayVideoInterface::stopPlay() {
    isQuiet.store(true);
    if (!slPlayItf) {
        (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_STOPPED);
    }
    while (!audioDeque.empty()) {
        audioDeque.pop_back();
    }
}



