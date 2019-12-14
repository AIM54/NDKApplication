//
// Created by Administrator on 2019/12/11.
//

#ifndef NDKAPPLICATION_JETPARTICULARDRAWER_H
#define NDKAPPLICATION_JETPARTICULARDRAWER_H

#include "BaseOpenGlDrawer.h"
#include <android/bitmap.h>

struct Particle {
    float postion[2];
    float velocity[2];
    float size;
    float curtime;
    float lifetime;
};

class JetParticularDrawer : public BaseOpenGlDrawer {

private:
    Particle dataArray[100];
public:
    JetParticularDrawer(JNIEnv *jniEnv, const _jobject *surface, const _jobject *pJobject);

};


#endif //NDKAPPLICATION_JETPARTICULARDRAWER_H
