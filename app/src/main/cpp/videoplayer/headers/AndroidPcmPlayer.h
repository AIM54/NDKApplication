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
public:
    std::mutex audioListMutex;
    std::condition_variable consumerCondition;
    std::condition_variable produceCondition;
    std::list<AudioFrameDataBean> audioFrameList;
protected:
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

    virtual ~AndroidPcmPlayer();
};


#endif //NDKAPPLICATION_ANDROIDPCMPLAYER_H
