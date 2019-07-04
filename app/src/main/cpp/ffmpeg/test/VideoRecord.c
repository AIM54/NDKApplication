//
// Created by Administrator on 2019/7/2.
//
#include <jni.h>
#include <libavformat/avformat.h>
#include <stdio.h>
#include <GlobalConfig.h>

JNIEXPORT void JNICALL
Java_com_bian_myapplication_utils_VideoUtil_papareForVideoRecord(JNIEnv *env, jclass type) {
    av_register_all();
}

#define INBUF_SIZE 4096

JNIEXPORT void JNICALL
Java_com_bian_myapplication_utils_VideoUtil_decodeImage(JNIEnv *env, jclass type,
                                                        jstring filePath_) {
    const char *filePath = (*env)->GetStringUTFChars(env, filePath_, 0);
    avcodec_register_all();
    FILE *file = fopen(filePath, "wb");
    AVCodec *jpegDecoder = avcodec_find_decoder(AV_CODEC_ID_JPEG2000);
    if (!jpegDecoder) {
        ALOGI("has'nt found the jpeg decoder");
        return;
    }
    AVCodecParserContext *avParser = av_parser_init(jpegDecoder->id);
    if (!avParser) {
        ALOGI("has'nt found the jpeg parse");
        return;
    }
    AVCodecContext *decoderContext = avcodec_alloc_context3(jpegDecoder);
    if (!decoderContext) {
        ALOGI("has'nt found the jpeg DecoderContext");
        goto end;
    }

    if (avcodec_open2(decoderContext, jpegDecoder, NULL) < 0) {
        ALOGI("can't open the file");
        return;
    }
    AVFrame *avFrame = av_frame_alloc();
    AVPacket *avPacket = av_packet_alloc();
    av_init_packet(avPacket);
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t data_size;
    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    while (!feof(file)) {
        data_size = fread(inbuf, 1, INBUF_SIZE, file);
        if (!data_size)
            break;
        data=inbuf;
    }
    end:
    avcodec_free_context(&decoderContext);
    avcodec_parameters_free(&avParser);

    (*env)->ReleaseStringUTFChars(env, filePath_, filePath);
}