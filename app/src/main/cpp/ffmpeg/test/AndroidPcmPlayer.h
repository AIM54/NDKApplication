//
// Created by Administrator on 2019/9/5.
//

#ifndef NDKAPPLICATION_ANDROIDPCMPLAYER_H
#define NDKAPPLICATION_ANDROIDPCMPLAYER_H

#include <list>
#include <SLES/OpenSLES.h>
#include <libavutil/channel_layout.h>
#include <SLES/OpenSLES_Android.h>
#include <mutex>
#include "AudioFrameDataBean.h"

class AndroidPcmPlayer {
protected:
    std::mutex audioListMutex;
    std::condition_variable audioCondition;
    std::condition_variable produceCondition;
    std::list<AudioFrameDataBean> audioFrameList;
    SLObjectItf playerObjItr;
    SLPlayItf slPlayItf;
    SLAndroidSimpleBufferQueueItf slAndroidSimpleBufferQueueItf;
    SLVolumeItf slVolumeItf;
    SLObjectItf slObjectItf1 = nullptr;
    SLEngineItf slEngine;
    SLObjectItf outputMixObject;
    SLEnvironmentalReverbItf outputMixoutputEnvironmentalReverbIter = nullptr;
    SLEffectSendItf slEffectSendItf;;
public:
    AndroidPcmPlayer();

    void play();

    void pause();

    void resume();

    void pushAudioFrameDatBean();

    int currentListSize();


    static void bqNewPlayerCallback(SLAndroidSimpleBufferQueueItf audioPlayQueue, void *context);

    void addAudioFrameData(AudioFrameDataBean &audioFrameDataBean);

    virtual ~AndroidPcmPlayer();
};


#endif //NDKAPPLICATION_ANDROIDPCMPLAYER_H
