//
// Created by Administrator on 2019/8/24.
//

#ifndef NDKAPPLICATION_VIDEOLIST_H
#define NDKAPPLICATION_VIDEOLIST_H

#include <stdbool.h>
#include "libavcodec/avcodec.h"

typedef struct ListEntity {
    AVFrame *value;
    struct ListEntity *next;
} ListEntity;

void initList();

void clearList();


bool isListEmpty();

int getListSize();

void pushAvFrame(AVFrame *avFrame);

AVFrame *popAvFrame();

#endif //NDKAPPLICATION_VIDEOLIST_H
