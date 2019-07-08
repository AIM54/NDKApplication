//
// Created by Administrator on 2019/7/8.
//

#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <stdio.h>

#define PICNUM 231

AVFormatContext *ifmt_ctx;
AVFormatContext *ofmt_ctx;
AVFrame *pframe;
AVPacket *pkt;
int video_index = 0;//标识视频流
int frame_index = 1;//标识帧号

const char *outfilename = "out.mkv";

int open_input_file(const char *filename) {
    ifmt_ctx = avformat_alloc_context();
    int ret = avformat_open_input(&ifmt_ctx, filename, NULL, NULL);
    if (ret < 0) {
        return -1;
    }
    ret = avformat_find_stream_info(ifmt_ctx, NULL);
    if (ret < 0) {
        return -1;
    }

    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            ret = avcodec_open2(ifmt_ctx->streams[i]->codec,
                                avcodec_find_decoder(ifmt_ctx->streams[i]->codec->codec_id), NULL);
            if (ret < 0) {
                return -1;
            }

        }
    }
    //av_dump_format(ifmt_ctx, 0, filename, 0);
    return 0;
}

int open_output_file(const char *filename) {

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, filename);
    if (!ofmt_ctx) {
        return -1;
    }

    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *ost = avformat_new_stream(ofmt_ctx, NULL);
        AVStream *ist = ifmt_ctx->streams[i];
        AVCodecContext *dec_ctx = ist->codec;
        AVCodecContext *enc_ctx = ost->codec;
        if (!ost) {
            return -1;
        }
        ost->time_base.num = 1;
        ost->time_base.den = 1000;
        AVCodec *encoder = NULL;
        AVDictionary *param = 0;

        if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
            enc_ctx->codec_id = ofmt_ctx->oformat->video_codec;
            encoder = avcodec_find_encoder(enc_ctx->codec_id);
            enc_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
            enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
            enc_ctx->bit_rate = 1500000;
            enc_ctx->width = dec_ctx->width;
            enc_ctx->height = dec_ctx->height;
            enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;

            enc_ctx->time_base = dec_ctx->time_base;//感觉不起作用
            enc_ctx->gop_size = 23;
            enc_ctx->qmin = -1;
            enc_ctx->qmax = -1;
            enc_ctx->max_b_frames = 3;
            enc_ctx->me_range = 16;
            enc_ctx->max_qdiff = 4;
            enc_ctx->qcompress = 0.6;
            av_dict_set(&param, "preset", "slow", 0);
            av_dict_set(&param, "tune", "zerolatency", 0);
            //av_dict_set(&param, "profile", "main", 0);
        }


        if (!encoder) {
            return -1;
        }
        int ret = avcodec_open2(enc_ctx, encoder, &param);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
            return ret;
        }
    }
    av_dump_format(ofmt_ctx, 0, filename, 1);

    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        ofmt_ctx->streams[video_index]->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        int ret = avio_open(&ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            return ret;
        }
    }
    /* init muxer, write output file header */
    int ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int write_frame() {
    int got_picture = 0;
    while (av_read_frame(ifmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_index) {
            AVCodecContext *dec_ctx = ifmt_ctx->streams[video_index]->codec;
            AVCodecContext *enc_ctx = ofmt_ctx->streams[video_index]->codec;
            int ret = avcodec_decode_video2(dec_ctx, pframe, &got_picture, pkt);
            if (ret < 0) {
                return -1;
            }
            if (got_picture) {
                got_picture = 0;
                av_free_packet(pkt);
                ret = avcodec_encode_video2(enc_ctx, pkt, pframe, &got_picture);
                if (ret < 0) {
                    return -1;
                }
                if (got_picture) {
                    goto write;
                } else {
                    goto end;
                }
            } else {
                goto end;
            }
            write:
            AVStream *in_stream = ifmt_ctx->streams[video_index];
            AVStream *out_stream = ofmt_ctx->streams[video_index];
            if (pkt->pts == AV_NOPTS_VALUE) {
                //Write PTS
                AVRational time_base1 = in_stream->time_base;
                //Duration between 2 frames (us)
                int64_t calc_duration = (double) AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
                //Parameters
                pkt->pts = (double) (frame_index * calc_duration) /
                           (double) (av_q2d(time_base1) * AV_TIME_BASE);
                pkt->dts = pkt->pts;
                pkt->duration =
                        (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
                frame_index++;
            }
            //Convert PTS/DTS
            pkt->pts = av_rescale_q_rnd(pkt->pts, in_stream->time_base, out_stream->time_base,
                                        (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt->dts = av_rescale_q_rnd(pkt->dts, in_stream->time_base, out_stream->time_base,
                                        (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt->duration = av_rescale_q(pkt->duration, in_stream->time_base,
                                         out_stream->time_base);
            pkt->pos = -1;
            ret = av_interleaved_write_frame(ofmt_ctx, pkt);
            if (ret < 0) {
                return -1;
            }
            end :
            av_free_packet(pkt);
        }
    }
    return 0;
}

int test() {
    av_register_all();
    pframe = av_frame_alloc();
    if (!pframe) {
        return -1;
    }
    pkt = av_packet_alloc();
    if (!pkt) {
        return -1;
    }

    //暂不支持载入png
    int ret = open_input_file("file_1.jpg");
    if (ret < 0) {
        return -1;
    }

    ret = open_output_file(outfilename);
    if (ret < 0) {
        return -1;
    }

    char filename[20] = {0};
    for (int i = 1; i < PICNUM; i++) {
        open_input_file(filename);
        write_frame();

        ret = av_write_trailer(ofmt_ctx);
        if (ret < 0) {
            return -1;
        }

        for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
            avcodec_close(ifmt_ctx->streams[i]->codec);
        }

        for (int i = 0; i < ofmt_ctx->nb_streams; i++) {
            avcodec_close(ofmt_ctx->streams[i]->codec);
        }

        av_frame_free(&pframe);
        av_free_packet(pkt);

        avformat_free_context(ifmt_ctx);
        avformat_free_context(ofmt_ctx);

        return 0;
    }
}