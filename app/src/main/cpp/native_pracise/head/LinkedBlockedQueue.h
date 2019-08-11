//
// Created by Administrator on 2019/8/6.
//

#ifndef NDKAPPLICATION_LINKEDBLOCKEDQUEUE_H
#define NDKAPPLICATION_LINKEDBLOCKEDQUEUE_H

#include <stdio.h>

typedef struct QueueEntity {
    struct QueueEntity *next;
    void *data;
} QueueEntity;

void pushData(void* data);
#endif //NDKAPPLICATION_LINKEDBLOCKEDQUEUE_H
