//
// Created by Administrator on 2019/9/10.
//

#ifndef NDKAPPLICATION_FIRSTOPENGLDRAWER_H
#define NDKAPPLICATION_FIRSTOPENGLDRAWER_H


#include <android/asset_manager.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

class FirstOpenGLDrawer {
private:
    AAssetManager *g_pAssetManager = nullptr;
    GLuint mProgramObject;
    int viewWidth, viewHeight;

public:
    void reize(int width,int height);

    void step();

    void setAssertManger(AAssetManager *manager);
};

char *getStringFromAssert(AAssetManager *aAssetManager, char *path);

GLuint loadShader(GLenum type, const char *shaderSrc);


#endif //NDKAPPLICATION_FIRSTOPENGLDRAWER_H
