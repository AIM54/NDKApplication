//
// Created by Administrator on 2019/7/8.
//
#include <GlobalConfig.h>
#include <jni.h>
#include <stdio.h>
#include <libavformat/avformat.h>

void getImageFrame(JNIEnv *env, jclass type,
                   jstring srcPath_,
                   jstring dstPath_);

JNINativeMethod method[] = {
        {"encodePic", "(Ljava/lang/String;Ljava/lang/String;)V", (void *) getImageFrame}
};


void getImageFrame(JNIEnv *env, jclass type,
                   jstring srcPath_,
                   jstring dstPath_) {
    const char *srcPath = (*env)->GetStringUTFChars(env, srcPath_, 0);
    const char *dstPath = (*env)->GetStringUTFChars(env, dstPath_, 0);

    av_register_all();
    AVFormatContext *avFormatContext = avformat_alloc_context();
    int status;
    if ((status = avformat_open_input(&avFormatContext, srcPath, NULL, NULL)) != 0) {
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        return;
    }
    int videoIndex = -1;
    for (int index = 0; index < avFormatContext->nb_streams; ++index) {
        if (avFormatContext->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = index;
            break;
        }
    }

    double videoLength = avFormatContext->streams[videoIndex]->duration *
                         av_q2d(avFormatContext->streams[videoIndex]->time_base);
    AVCodec *avCodec = avcodec_find_decoder(
            avFormatContext->streams[videoIndex]->codecpar->codec_id);

    if (!avCodec) {
        goto end;
    }
    AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);
    if (!avCodecContext) {
        goto end;
    }
    int decodeStatus = avcodec_parameters_to_context(avCodecContext,
                                                     avFormatContext->streams[videoIndex]->codecpar);
    if (decodeStatus < 0) {
        goto end;
    }
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        goto end;
    }
    AVPacket *avPacket = av_packet_alloc();
    av_init_packet(avPacket);
    ALOGI("可以开始解析图片了:");
    while (av_read_frame(avFormatContext, avPacket) > 0) {

    }
    (*env)->ReleaseStringUTFChars(env, dstPath_, dstPath);
    (*env)->ReleaseStringUTFChars(env, srcPath_, srcPath);
    end:
    avformat_close_input(&avFormatContext);
    avcodec_free_context(&avCodecContext);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {

}

int registerNativeMethod(JNIEnv *pInterface) {
    jclass jobject = (*pInterface)->FindClass(pInterface,
                                              "com/bian/myapplication/utils/VideoUtil");
    if (!jobject) {
        return -1;
    }
    if ((*pInterface)->RegisterNatives(pInterface, jobject, method,
                                       sizeof(method) / sizeof(method[0])) < 0) {
        return -1;
    }
    return JNI_OK;
}