//
// Created by Administrator on 2019/11/15.
//

#include "MainControl.h"
#include<jni.h>
#include "BitmapCompress.h"

extern "C" {
JNINativeMethod firstGlMethod[] = {
        {"compressBitmap", "(Ljava/lang/String;Landroid/graphics/Bitmap;)I", (void *) compressBitmap},
};
}

void JNI_OnUnload(JavaVM *vm, void *reserved) {

}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

int registerNativeMethod(JNIEnv *pInterface) {
    jclass jobject = pInterface->FindClass("com/bian/myapplication/utils/BitMapCompressUtil");
    if (!jobject) {
        return -1;
    }
    if (pInterface->RegisterNatives(jobject, firstGlMethod,
                                    sizeof(firstGlMethod) / sizeof(firstGlMethod[0])) < 0) {
        return -1;
    }
    return JNI_OK;
}