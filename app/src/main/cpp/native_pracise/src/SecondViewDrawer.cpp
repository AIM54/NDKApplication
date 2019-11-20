//
// Created by Administrator on 2019/11/20.
//

#include "SecondViewDrawer.h"
extern "C" {
#include "AssetReader.h"
}
SecondViewDrawer::SecondViewDrawer(JNIEnv *jniEnv, const _jobject *surface,
                                   const _jobject *pJobject) : BaseOpenGlDrawer(jniEnv,
                                           const_cast<jobject>(surface),const_cast<jobject>(pJobject)) {

}
int SecondViewDrawer::init() {
    BaseOpenGlDrawer::init();
    return 0;
}
