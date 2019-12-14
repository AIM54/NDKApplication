//
// Created by Administrator on 2019/12/11.
//

#include "JetParticularDrawer.h"

JetParticularDrawer::JetParticularDrawer(JNIEnv *jniEnv, const _jobject *surface,
                                         const _jobject *pJobject) : BaseOpenGlDrawer(jniEnv,
                                                                                      const_cast<jobject>(surface),
                                                                                      const_cast<jobject>(pJobject)) {
}
