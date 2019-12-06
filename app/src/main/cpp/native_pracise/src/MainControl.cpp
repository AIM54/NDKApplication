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
#include "PointDrawer.h"
#include "SurfaceViewDrawer.h"
#include "BaseOpenGlDrawer.h"
#include "SecondViewDrawer.h"
#include "BitmapDrawer.h"
#include "CubeDrawer.h"
#include "Instancing.h"
#include "esUtil.h"

#include "NewCuberDrawer.h"

ESContext *esContext = nullptr;
extern "C" {
JNINativeMethod firstGlMethod[] = {
        {"init",        "()V",                                   (void *) initGL},
        {"resize",      "(II)V",                                 (void *) resizeGL},
        {"step",        "()V",                                   (void *) step},
        {"initAsserts", "(Landroid/content/res/AssetManager;)V", (void *) setAssertManager}

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
    esContext = static_cast<ESContext *>(malloc(sizeof(ESContext)));
}

void resizeGL(JNIEnv *jniEnv, jobject jobj, jint width, jint height) {
    ALOGI("resize");
}

void step(JNIEnv *jniEnv, jobject jobj) {
}

void setAssertManager(JNIEnv *jniEnv, jobject claz, jobject assertManager) {
    g_pAssetManager = AAssetManager_fromJava(jniEnv, assertManager);
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
        case 3:
            ALOGI("init CubeDrawer:%d", type);
            baseOpenGlDrawer = new NewCuberDrawer(env, surface, assertManager);
            break;
        default:
            ALOGI("init SecondViewDrawer:%d", type);
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