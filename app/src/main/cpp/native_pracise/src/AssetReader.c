//
// Created by Administrator on 2019/11/15.
//

#include "AndroidLog.h"
#include "AssetReader.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

char *readStringFromAssert(AAssetManager *aAssetManager, char *path) {
    AAsset *aasert = AAssetManager_open(aAssetManager, path, AASSET_MODE_UNKNOWN);
    off_t assetSize = AAsset_getLength(aasert);
    char *buffer = malloc(sizeof(char) * (assetSize + 1));
    memset(buffer, 0, assetSize + 1);
    int readStatus = AAsset_read(aasert, buffer, assetSize);
    AAsset_close(aasert);
    if (readStatus >= 0) {
    }
    return buffer;
}

GLuint loadShader(GLenum type, const char *shaderSrc){
    GLuint shader;
    GLint compiled;
    shader = glCreateShader(type);
    if (!shader) {
        return 0;
    }
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint inforLen = 0;
        //获取失败的日志信息
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &inforLen);
        if (inforLen > 0) {
            char *inforLog = malloc(inforLen * sizeof(char));
            glGetShaderInfoLog(shader, inforLen, NULL, inforLog);
            free(inforLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


