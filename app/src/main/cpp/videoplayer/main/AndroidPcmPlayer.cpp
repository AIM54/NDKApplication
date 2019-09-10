//
// Created by Administrator on 2019/9/5.
//

#include <GlobalConfig.h>
#include "AndroidPcmPlayer.h"
#include <mutex>

using namespace std;

AndroidPcmPlayer *thisPlayer = nullptr;

AndroidPcmPlayer::AndroidPcmPlayer() {
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
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
        return;
    }

    sLresult = (*slEngine)->CreateAudioPlayer(slEngine, &playerObjItr, &slDataSource, &slDataSink,
                                              3,
                                              playerIds, playReq);
    if (sLresult != SL_RESULT_SUCCESS) {
        ALOGI("CreateAudioFailed the reson is :%d", sLresult);
        return;
    }

    (*playerObjItr)->Realize(playerObjItr, SL_BOOLEAN_FALSE);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_PLAY, &slPlayItf);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_BUFFERQUEUE, &slAndroidSimpleBufferQueueItf);

    (*slAndroidSimpleBufferQueueItf)->RegisterCallback(slAndroidSimpleBufferQueueItf,
                                                       bqNewPlayerCallback,
                                                       nullptr);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_EFFECTSEND, &slEffectSendItf);

    (*playerObjItr)->GetInterface(playerObjItr, SL_IID_VOLUME, &slVolumeItf);
}

void AndroidPcmPlayer::bqNewPlayerCallback(SLAndroidSimpleBufferQueueItf audioPlayQueue,
                                           void *context) {
    unique_lock<mutex> androidListLock(thisPlayer->audioListMutex);
    thisPlayer->consumerCondition.wait(androidListLock,
                                    [] { return !thisPlayer->audioFrameList.empty(); });
    AudioFrameDataBean audioFrameDataBean = thisPlayer->audioFrameList.front();
    (*audioPlayQueue)->Enqueue(audioPlayQueue, audioFrameDataBean.getData(),
                               audioFrameDataBean.getSize());
    ALOGI("the displayTime in ConsumerThread:%f", audioFrameDataBean.getTimeStamp());
    thisPlayer->audioFrameList.pop_front();
    thisPlayer->produceCondition.notify_all();
}


void AndroidPcmPlayer::play() {
    thisPlayer = this;
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
    bqNewPlayerCallback(slAndroidSimpleBufferQueueItf, nullptr);
}

AndroidPcmPlayer::~AndroidPcmPlayer() {

}
