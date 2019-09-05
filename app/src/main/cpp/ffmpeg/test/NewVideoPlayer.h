//
// Created by Administrator on 2019/9/3.
//

#ifndef NDKAPPLICATION_NEWVIDEOPLAYER_H
#define NDKAPPLICATION_NEWVIDEOPLAYER_H


#include "MutilThreadPlayer.h"

class NewVideoPlayer : public MutilThreadPlayer {
public:
    virtual int seekToFrame(int senconds, bool allowAround);

    virtual int pauseTheVideo();

    virtual int resumeTheVideo();

protected:
    virtual int displayVideo(JNIEnv *jniEnv, jobject surfaceView) override;

};

#endif //NDKAPPLICATION_NEWVIDEOPLAYER_H
