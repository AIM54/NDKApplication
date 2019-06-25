//
// Created by Administrator on 2019/6/20.
//
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <GlobalConfig.h>
#include <stdbool.h>


void beginDecodeVideo(AVFormatContext *pContext, int index, JNIEnv *pInterface, jobject pVoid);

int decodeVideoFrame(AVFormatContext *pContext, AVPacket *pPacket, AVFrame *pFrame,
                     ANativeWindow_Buffer *buffer, ANativeWindow *pWindow,
                     struct SwsContext *pSwsContext);

JNIEXPORT void JNICALL
Java_com_bian_myapplication_utils_VideoUtil_testSoLibrary(JNIEnv *env, jclass type) {
    jclass jcls = (*env)->FindClass(env, "com/bian/myapplication/utils/VideoUtil");
    jmethodID jMethodId = (*env)->GetStaticMethodID(env, jcls, "log", "(Ljava/lang/String;)V");
    char *message = "tomcat";
    jstring jMsg = (*env)->NewStringUTF(env, message);
    (*env)->CallStaticVoidMethod(env, jcls, jMethodId, jMsg);
}


JNIEXPORT void JNICALL
Java_com_bian_myapplication_utils_VideoUtil_decodeVideo(JNIEnv *env, jclass type, jstring filePath_,
                                                        jobject surface) {
    const char *file_name = (*env)->GetStringUTFChars(env, filePath_, 0);
    av_register_all();

    AVFormatContext * pFormatCtx = avformat_alloc_context();

    // Open video file
    if(avformat_open_input(&pFormatCtx, file_name, NULL, NULL)!=0) {

        return ; // Couldn't open file
    }

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0) {
        return ;
    }

    // Find the first video stream
    int videoStream = -1, i;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO
            && videoStream < 0) {
            videoStream = i;
        }
    }
    if(videoStream==-1) {
        return ; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    AVCodecContext  * pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    AVCodec * pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {

        return ;; // Codec not found
    }

    if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        return ;; // Could not open codec
    }

    // 获取native window
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);

    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;

    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow,  videoWidth, videoHeight, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0) {
        return ; // Could not open codec
    }

    // Allocate video frame
    AVFrame * pFrame = av_frame_alloc();

    // 用于渲染
    AVFrame * pFrameRGBA = av_frame_alloc();
    if(pFrameRGBA == NULL || pFrame == NULL) {
        return ;
    }

    // Determine required buffer size and allocate buffer
    // buffer中数据就是用于渲染的,且格式为RGBA
    int numBytes=av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height, 1);
    uint8_t * buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    int frameFinished;
    AVPacket packet;
    while(av_read_frame(pFormatCtx, &packet)>=0) {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) {

            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // 并不是decode一次就可解码出一帧
            if (frameFinished) {

                // lock native window buffer
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

                // 格式转换
                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                // 获取stride
                uint8_t * dst = windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t * src = (uint8_t*) (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }

        }
        av_packet_unref(&packet);
    }

    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);
    (*env)->ReleaseStringUTFChars(env, filePath_, file_name);
}


void beginDecodeVideo(AVFormatContext *pContext, int index, JNIEnv *env, jobject holder) {
    AVCodec *videoCodec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    if (!videoCodec) {
        return;
    }
    AVCodecContext *codecContext = avcodec_alloc_context3(videoCodec);
    if (!codecContext) {
        return;
    }
    __android_log_print(ANDROID_LOG_INFO, LOG_MARK, "before_avcodec_parameters_to_context");

    if (avcodec_parameters_to_context(videoCodec, codecContext) < 0) {
        return;
    }
    __android_log_print(ANDROID_LOG_INFO, LOG_MARK, "benfore_avcodec_opendfa2");
    __android_log_print(ANDROID_LOG_INFO, LOG_MARK, "benfore_avcodec_open2：videoDecorderName: %s",
                        videoCodec->long_name);
    __android_log_print(ANDROID_LOG_INFO, LOG_MARK, "benfore_avcodec_open2_VideoHeight:%d",
                        codecContext->height);
    if (avcodec_open2(codecContext, videoCodec, NULL) < 0) {
        return;
    }
    __android_log_print(ANDROID_LOG_INFO, LOG_MARK, "avcodec_open2");

    AVFrame *srcFrame = av_frame_alloc();
    AVFrame *dstFrame = av_frame_alloc();
    __android_log_print(ANDROID_LOG_INFO, LOG_MARK, "after_alloc_frame");

    int videoWidth = codecContext->width;
    int videoHeight = codecContext->height;
    //确定所需缓冲区大小并分配缓冲区内存空间
    //Buffer 中的数据用于渲染
    int numberBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, videoWidth, videoHeight, 1);
    if (avcodec_open2(codecContext, videoCodec, NULL) < 0) {
        return;
    }
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, holder);
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    __android_log_print(ANDROID_LOG_INFO, LOG_MARK, "after_Create_native_Window");

    uint8_t *bufferSize = av_malloc(numberBytes * sizeof(uint8_t));
    av_image_fill_arrays(dstFrame->data, dstFrame->linesize, bufferSize, AV_PIX_FMT_RGBA,
                         codecContext->width, codecContext->height, 0);
    struct SwsContext *swsContext = sws_getContext(videoWidth, videoHeight, codecContext->pix_fmt,
                                                   videoWidth, videoHeight, AV_PIX_FMT_RGBA,
                                                   SWS_BILINEAR, NULL, NULL, NULL);
    AVPacket avPacket;
    av_init_packet(&avPacket);
    ANativeWindow_Buffer windowBuffer;
    __android_log_print(ANDROID_LOG_INFO, LOG_MARK, "after_init_avPacket");

    while (av_read_frame(pContext, &avPacket) >= 0) {
        if (avPacket.stream_index == index) {
            int ret = avcodec_send_packet(pContext, &avPacket);
            if (ret < 0) {
                break;
            }
            while (true) {
                ret = avcodec_receive_frame(pContext, srcFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if (ret < 0) {
                    fprintf(stderr, "Error during decoding\n");
                    break;
                }
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
                //格式转换
                sws_scale(swsContext, (const uint8_t *const *) srcFrame->data, srcFrame->linesize,
                          0, codecContext->height,
                          dstFrame->data, dstFrame->linesize);
                uint8_t *dst = windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = dstFrame->data[0];
                int srcStride = dstFrame->linesize[0];
                for (int h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }
                ANativeWindow_unlockAndPost(nativeWindow);
            }
        }
    }
    av_packet_free(&avPacket);
    av_free(dstFrame);
    av_free(srcFrame);
    avcodec_close(codecContext);
}



