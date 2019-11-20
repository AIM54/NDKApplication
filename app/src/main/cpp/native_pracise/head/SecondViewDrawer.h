//
// Created by Administrator on 2019/11/20.
//

#ifndef NDKAPPLICATION_SECONDVIEWDRAWER_H
#define NDKAPPLICATION_SECONDVIEWDRAWER_H


#include "BaseOpenGlDrawer.h"

class SecondViewDrawer : public BaseOpenGlDrawer {
public:
    SecondViewDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject);

    int init() override;

};


#endif //NDKAPPLICATION_SECONDVIEWDRAWER_H
