//
// Created by Administrator on 2019/9/10.
//

#include "FirstOpenGLDrawer.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <AndroidLog.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

FirstOpenGLDrawer::FirstOpenGLDrawer() {

}

void FirstOpenGLDrawer::reize() {

}

void FirstOpenGLDrawer::step() {

}

void FirstOpenGLDrawer::setAssertManger(AAssetManager *manager) {
    this->g_pAssetManager = manager;
    getStringFromAssert(g_pAssetManager, "first_shader.glsl");
    getStringFromAssert(g_pAssetManager, "first_v.glsl");
}

char *getStringFromAssert(AAssetManager *aAssetManager, char *path) {
    AAsset *aasert = AAssetManager_open(aAssetManager, path, AASSET_MODE_UNKNOWN);
    off_t assetSize = AAsset_getLength(aasert);
    char *buffer = new char[assetSize];
    int readStatus = AAsset_read(aasert, buffer, assetSize);
    AAsset_close(aasert);
    if (readStatus >= 0) {
        ALOGI("the read infor: %s", buffer);
    }
    return buffer;
}