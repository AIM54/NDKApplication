//
// Created by hejunlin on 17/5/6.
//
#ifndef FFMPEG_CORE_H
#define FFMPEG_CORE_H

#include <stdio.h>
#include <stdint.h>

int initFFmpeg(int *rate, int *channel, char *url);
int getPCM(void **pcm, size_t * pcmSize);
int releaseFFmpeg();

#endif //FFMPEG_CORE_H
