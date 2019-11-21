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
#include <cstring>

extern "C" {
#include "AssetReader.h"
}

void FirstOpenGLDrawer::reize(int width, int height) {
    viewWidth = width;
    viewHeight = height;
}

void FirstOpenGLDrawer::step() {
    GLfloat vVertices[] = {0.0f, 0.5f, 0.0f,
                           -0.5f, -0.5f, 0.0f,
                           0.5f, -0.5f, 0.0f
    };
    // Set the viewport
    glViewport(0, 0, viewWidth, viewHeight);
    //clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //use the programObject;
    glUseProgram(mProgramObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);

}

void FirstOpenGLDrawer::setAssertManger(AAssetManager *manager) {
    this->g_pAssetManager = manager;
    char *shaderString = readStringFromAssert(g_pAssetManager, "first_shader.glsl");
    char *vectorString = readStringFromAssert(g_pAssetManager, "first_v.glsl");
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    vertexShader = loadShader(GL_VERTEX_SHADER, vectorString);
    if (!vertexShader) {
        return;
    }

    fragmentShader = loadShader(GL_FRAGMENT_SHADER, shaderString);
    if (!fragmentShader) {
        return;
    }

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
    glClearColor(1.0f, 1.0f, 0.0f, 0.0f);
    mProgramObject = programObject;
    ALOGI("after init gl");
    delete[]shaderString;
    delete[]vectorString;
}

