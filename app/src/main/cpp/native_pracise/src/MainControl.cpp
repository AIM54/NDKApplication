//
// Created by Administrator on 2019/8/6.
//

#include <jni.h>
#include <list>
#include <iostream>
#include <stdio.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

#include "MainControl.h"
#include "FirstOpenGLDrawer.h"
#include "AndroidLog.h"
#include "SimpleBean.h"
#include "PointDrawer.h"
#include "SurfaceViewDrawer.h"
#include "BaseOpenGlDrawer.h"
#include "SecondViewDrawer.h"
#include "BitmapDrawer.h"
#include "ImageDrawer.h"

extern "C" {
JNINativeMethod firstGlMethod[] = {
        {"init",        "()V",                                   (void *) initGL},
        {"resize",      "(II)V",                                 (void *) resizeGL},
        {"step",        "()V",                                   (void *) step},
        {"initAsserts", "(Landroid/content/res/AssetManager;)V", (void *) setAssertManager},
        {"nTestNew",    "()V",                                   (void *) testNewObject},
        {"nTestDelete", "()V",                                   (void *) testDeleteObject}

};

JNINativeMethod openglMethod[] = {
        {"initSurfaceView",   "(Landroid/view/Surface;Landroid/content/res/AssetManager;)V",                           (void *) initSurfaceGL},
        {"initSurfaceView",   "(Landroid/view/Surface;Landroid/content/res/AssetManager;I)V",                          (void *) initSurfaceGLByType},
        {"initPicture",       "(Landroid/view/Surface;Landroid/content/res/AssetManager;ILjava/lang/String;)V",        (void *) initPictureSurface},
        {"initPicture",       "(Landroid/view/Surface;Landroid/content/res/AssetManager;ILandroid/graphics/Bitmap;)V", (void *) initBitmapSurface},
        {"resizeSurfaceView", "(II)V",                                                                                 (void *) resizeSurfaceGL},
        {"stepSurfaceView",   "()V",                                                                                   (void *) stepSurface},
        {"destroyView",       "()V",                                                                                   (void *) destroyDrawer}
};
}

FirstOpenGLDrawer *firstOpenGLDrawer = nullptr;

AAssetManager *g_pAssetManager = nullptr;

SimpleBean *simpleBean = nullptr;

SurfaceViewDrawer *surfaceViewDrawer = nullptr;

BaseOpenGlDrawer *baseOpenGlDrawer = nullptr;


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

void JNI_OnUnload(JavaVM *vm, void *reserved) {
    vm->DestroyJavaVM();
}

int registerNativeMethod(JNIEnv *pInterface) {
    jclass jobject = pInterface->FindClass("com/bian/myapplication/utils/NativePracise");
    if (!jobject) {
        return -1;
    }
    if (pInterface->RegisterNatives(jobject, firstGlMethod,
                                    sizeof(firstGlMethod) / sizeof(firstGlMethod[0])) < 0) {
        return -1;
    }

    jclass jobject2 = pInterface->FindClass("com/bian/myapplication/utils/SurfaceDrawer");
    if (!jobject2) {
        ALOGI("failed to find class");
        return -1;
    }
    if (pInterface->RegisterNatives(jobject2, openglMethod,
                                    sizeof(openglMethod) / sizeof(openglMethod[0])) < 0) {
        ALOGI("failed to registerMethods");
        return -1;
    }
    return JNI_OK;
}


void initGL(JNIEnv *env, jobject jobj) {
    firstOpenGLDrawer = new FirstOpenGLDrawer();
    firstOpenGLDrawer->setAssertManger(g_pAssetManager);
}

void resizeGL(JNIEnv *jniEnv, jobject jobj, jint width, jint height) {
    ALOGI("resize");
    firstOpenGLDrawer->reize(width, height);
}

void setAssertManager(JNIEnv *jniEnv, jobject claz, jobject assertManager) {
    ALOGI("setAssertManager");
    g_pAssetManager = AAssetManager_fromJava(jniEnv, assertManager);

}

void step(JNIEnv *jniEnv, jobject jobj) {
    ALOGI("step");
    firstOpenGLDrawer->step();
}

void testNewObject() {
    if (!simpleBean) {
        simpleBean = new SimpleBean();
    }
}

void testDeleteObject() {
    if (simpleBean) {
        delete simpleBean;
        simpleBean = nullptr;
    }
}

void initSurfaceGL(JNIEnv *env, jobject jobj, jobject surface, jobject assertManager) {
    if (!surfaceViewDrawer) {
        surfaceViewDrawer = new SurfaceViewDrawer(env, surface, assertManager);
    }
    surfaceViewDrawer->init();
}

void
initSurfaceGLByType(JNIEnv *env, jobject jobj, jobject surface, jobject assertManager, jint type) {
    if (baseOpenGlDrawer) {
        destroyDrawer();
    }
    switch (type) {
        case 1:
            baseOpenGlDrawer = new SecondViewDrawer(env, surface, assertManager);
            break;
        case 2:
            baseOpenGlDrawer = new PointDrawer(env, surface, assertManager);
            break;
        default:
            baseOpenGlDrawer = new SecondViewDrawer(env, surface, assertManager);
            break;
    }

    baseOpenGlDrawer->init();
}

void
initPictureSurface(JNIEnv *env, jobject jobj, jobject surface, jobject assertManager, jint type,
                   jstring imagePath_) {
    const char *imagePath = env->GetStringUTFChars(imagePath_, 0);
    ALOGI("the Picture path is:%s", imagePath);
    if (baseOpenGlDrawer) {
        destroyDrawer();
    }
    baseOpenGlDrawer = new ImageDrawer(env, surface, assertManager, const_cast<char *>(imagePath));
    baseOpenGlDrawer->init();
}

void
initBitmapSurface(JNIEnv *env, jobject jobj, jobject surface, jobject assertManager, jint type,
                  jobject bitmap) {
    if (baseOpenGlDrawer) {
        destroyDrawer();
    }
    baseOpenGlDrawer = new BitmapDrawer(env, surface, assertManager, bitmap);
    baseOpenGlDrawer->init();
}

void resizeSurfaceGL(JNIEnv *jniEnv, jobject jobj, int width, int height) {
    if (surfaceViewDrawer) {
        surfaceViewDrawer->resize(width, height);
    }
    if (baseOpenGlDrawer) {
        baseOpenGlDrawer->resize(width, height);
    }
}

void stepSurface(JNIEnv *jniEnv, jobject jobj) {
    if (surfaceViewDrawer) {
        surfaceViewDrawer->step();
    }
    if (baseOpenGlDrawer) {
        baseOpenGlDrawer->step();
    }
}

void destroyDrawer() {
    if (surfaceViewDrawer) {
        delete surfaceViewDrawer;
        surfaceViewDrawer = nullptr;
    }
    if (baseOpenGlDrawer) {
        delete baseOpenGlDrawer;
        baseOpenGlDrawer = nullptr;
    }
}