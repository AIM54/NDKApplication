//
// Created by Administrator on 2019/8/31.
//

#ifndef NDKAPPLICATION_AUDIOPLAYER_H
#define NDKAPPLICATION_AUDIOPLAYER_H

#include <string>
#include <SLES/OpenSLES_Android.h>
#include <list>
#include "AudioFrameDataBean.h"

class AudioPlayer {

public:
    int playAudio(char *url);

    friend void bqNewPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

private:
    int decodeAudioData(char *url);

    /**
     * 打开多媒体文件
     * @param url  路径
     * @return
     */
    int openInputUrl(char *url);

    int playAudioAndroid();

    std::list<AudioFrameDataBean> audioFrameList;

    void pushAudioFrameIntoList(AudioFrameDataBean *audioFrameDataBean);

};

void bqNewPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

#endif //NDKAPPLICATION_AUDIOPLAYER_H
