//
// Created by Administrator on 2019/12/8.
//

#ifndef NDKAPPLICATION_NEWPARTICULARDRAWER_H
#define NDKAPPLICATION_NEWPARTICULARDRAWER_H


#include "BaseOpenGlDrawer.h"

class NewParticularDrawer : public BaseOpenGlDrawer {
public:
    NewParticularDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject);

    int init() override;

    void step() override;
};


#endif //NDKAPPLICATION_NEWPARTICULARDRAWER_H
