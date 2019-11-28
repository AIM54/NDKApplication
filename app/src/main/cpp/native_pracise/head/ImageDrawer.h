//
// Created by Administrator on 2019/11/22.
//

#ifndef NDKAPPLICATION_IMAGEDRAWER_H
#define NDKAPPLICATION_IMAGEDRAWER_H

#include "BaseOpenGlDrawer.h"

class ImageDrawer : public BaseOpenGlDrawer {

private:
    char *imagePath;
public:
    ImageDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject, char *path);

    void step() override;

    int init() override ;
};


#endif //NDKAPPLICATION_IMAGEDRAWER_H
