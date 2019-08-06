//
// Created by Administrator on 2019/8/5.
//

#ifndef NDKAPPLICATION_MYVIDEOPLAYER_H
#define NDKAPPLICATION_MYVIDEOPLAYER_H

#include <MyVideoPlayer.h>
#include <jni.h>
#include <string>

using std::string;

int (*videoPlayFunction[4])(string videoUrl, int time);


class MyVideoPlayer {
};


#endif //NDKAPPLICATION_MYVIDEOPLAYER_H
