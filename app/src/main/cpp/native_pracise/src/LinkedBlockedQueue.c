//
// Created by Administrator on 2019/8/6.
//
#include "LinkedBlockedQueue.h"
#include <malloc.h>

QueueEntity *head;
QueueEntity *foot;
int size;

void pushData(void *data) {
    if (!head) {
        head = malloc(sizeof(QueueEntity));
        head->data = data;
    } else if (!foot) {
        foot = malloc(sizeof(QueueEntity));
        foot->data=head->data;
    }else{
    }
}

