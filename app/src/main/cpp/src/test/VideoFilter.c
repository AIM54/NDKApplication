//
// Created by Administrator on 2019/6/30.
//
#include <jni.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavutil/opt.h>

int openInputFile(AVFormatContext *avFormatContext, const char *path);


int initAvFilter(AVFormatContext *avFormatContext);

static int video_stream_index = -1;


AVCodecContext *avDecoderContext;

AVFilterContext *buffersrc_ctx;

AVFilterContext *bufferdst_ctx;

void JNICALL
Java_com_bian_myapplication_utils_VideoUtil_addTextToVideo(JNIEnv *env, jclass type,
                                                           jstring filePath_, jstring markText_,
                                                           jstring tempPath_) {
    const char *filePath = (*env)->GetStringUTFChars(env, filePath_, 0);
    const char *markText = (*env)->GetStringUTFChars(env, markText_, 0);
    const char *tempPath = (*env)->GetStringUTFChars(env, tempPath_, 0);
    av_register_all();
    avfilter_register_all();
    AVFrame *videoFrame = av_frame_alloc();
    AVFrame *filterFrame = av_frame_alloc();
    AVFormatContext *avFormatContext = avformat_alloc_context();
    if (openInputFile(avFormatContext, filePath) < 0) {
        goto end;
    }
    if (initAvFilter(avFormatContext) < 0) {

    }
    end:
    (*env)->ReleaseStringUTFChars(env, filePath_, filePath);
    (*env)->ReleaseStringUTFChars(env, markText_, markText);
    (*env)->ReleaseStringUTFChars(env, tempPath_, tempPath);

}

int initAvFilter(AVFormatContext *avFormatContext) {
    int ret = 0;
    char args[512];
    AVFilter *bufferFilter = avfilter_get_by_name("buffer");
    AVFilter *bufferSinkFilter = avfilter_get_by_name("buffersink");
    AVFilterInOut *avFilterInput = avfilter_inout_alloc();
    AVFilterInOut *avFilterOutput = avfilter_inout_alloc();
    AVRational videoTimeBase = avFormatContext->streams[video_stream_index]->time_base;
    enum AVPixelFormat avpixFormat[] = {AV_PIX_FMT_RGBA, AV_PIX_FMT_NB};
    AVFilterGraph *avFilterGraph = avfilter_graph_alloc();
    if (!avFilterInput || !avFilterOutput || !avFilterGraph) {
        ret = AVERROR(ENOMEM);
    }
    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             avDecoderContext->width, avDecoderContext->height, avDecoderContext->pix_fmt,
             videoTimeBase.num, videoTimeBase.den,
             avDecoderContext->sample_aspect_ratio.num, avDecoderContext->sample_aspect_ratio.den);
    ret = avfilter_graph_create_filter(&buffersrc_ctx, bufferFilter, "in", args, NULL,
                                       avFilterGraph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
    }
    ret = avfilter_graph_create_filter(&bufferdst_ctx, bufferSinkFilter, "out", args, NULL,
                                       avFilterGraph);

    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
    }

    ret = av_opt_set_int_list(buffersrc_ctx, "pix_fmts", avpixFormat,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
    }
    avFilterOutput->filter_ctx=bufferdst_ctx;


    return ret;
}


JNIEXPORT int openInputFile(AVFormatContext *avFormatContext, const char *path) {
    int status = -1;
    AVCodec *avCodec;

    if ((status = avformat_open_input(&avFormatContext, path, NULL, NULL)) < 0) {
        return status;
    }
    if ((status = avformat_find_stream_info(avFormatContext, NULL)) < 0) {
        return status;
    }
    if ((status = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &avCodec, 0)) <
        0) {
        return status;
    }
    video_stream_index = status;
    AVCodecContext *avCodeContext = avcodec_alloc_context3(avCodec);
    if (!avCodeContext) {
        return -1;
    }
    avDecoderContext = avCodeContext;
    AVCodecParameters *avCodecParameters = avcodec_parameters_alloc();

    if ((status = avcodec_parameters_from_context(avCodecParameters, avCodeContext)) < 0) {
        return status;
    }

    return avcodec_open2(avCodeContext, avCodec, NULL);
}