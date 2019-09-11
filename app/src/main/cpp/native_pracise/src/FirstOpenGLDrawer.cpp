//
// Created by Administrator on 2019/9/10.
//

#include "FirstOpenGLDrawer.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <AndroidLog.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <malloc.h>

void FirstOpenGLDrawer::reize(int width, int height) {
    viewWidth = width;
    viewHeight = height;
}

void FirstOpenGLDrawer::step() {
    GLfloat vVertices[] = {0.0f, 0.5f, 0.0f,
                           -0.5f, -0.5f, 0.0f,
                           0.5f, -0.5f, 0.0f
    };
    ALOGI("the viewWidth:%d||the viewHeight:%d", viewWidth, viewHeight);
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);
    //clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //use the programObject;
    glUseProgram(mProgramObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

}

void FirstOpenGLDrawer::setAssertManger(AAssetManager *manager) {
    this->g_pAssetManager = manager;
    char *shaderString = getStringFromAssert(g_pAssetManager, "first_shader.glsl");
    char *vectorString = getStringFromAssert(g_pAssetManager, "first_v.glsl");
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    vertexShader = loadShader(GL_VERTEX_SHADER, vectorString);
    if (!vertexShader) {
        return;
    }
    delete[]vectorString;
    fragmentShader = loadShader(GL_FRAGMENT_SHADER, shaderString);
    if (!fragmentShader) {
        return;
    }
    delete[]shaderString;
    programObject = glCreateProgram();
    if (!programObject) {
        return;
    }
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glLinkProgram(programObject);
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint inforLen = 0;
        glGetShaderiv(programObject, GL_INFO_LOG_LENGTH, &inforLen);
        if (inforLen > 0) {
            char *logInfor = static_cast<char *>(malloc(inforLen * sizeof(char)));
            glGetShaderInfoLog(programObject, inforLen, nullptr, logInfor);
            ALOGE("link the opengl failed:%s", logInfor);
            free(logInfor);
        }
        glDeleteProgram(programObject);
        return;
    }
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    mProgramObject = programObject;
    ALOGI("after init gl");
}

GLuint loadShader(GLenum type, const char *shaderSrc) {
    GLuint shader;
    GLint compiled;
    shader = glCreateShader(type);
    if (!shader) {
        return 0;
    }
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint inforLen = 0;
        //获取失败的日志信息
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &inforLen);
        if (inforLen > 0) {
            char *inforLog = static_cast<char *>(malloc(inforLen * sizeof(char)));
            glGetShaderInfoLog(shader, inforLen, nullptr, inforLog);
            ALOGE("loadShaderError:%s", inforLog);
            free(inforLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
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