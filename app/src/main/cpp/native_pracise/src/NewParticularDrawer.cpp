//
// Created by Administrator on 2019/12/8.
//

#include "NewParticularDrawer.h"

NewParticularDrawer::NewParticularDrawer(JNIEnv *jniEnv, const _jobject *surface,
                                         const _jobject *pJobject) : BaseOpenGlDrawer(jniEnv,
                                                                                      const_cast<jobject>(surface),
                                                                                      const_cast<jobject>(pJobject)) {

}

int NewParticularDrawer::init() {

}

void NewParticularDrawer::step() {

}
