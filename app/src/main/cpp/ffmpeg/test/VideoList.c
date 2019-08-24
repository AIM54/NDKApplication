//
// Created by Administrator on 2019/8/24.
//
#include "VideoList.h"
#include <malloc.h>

ListEntity *head;
ListEntity *tail;

int listSize;

void initList() {
    head = malloc(sizeof(ListEntity));
    head->value = NULL;
    tail = head;
    listSize = 0;
}

bool isListEmpty() {
    return head == tail;
}

int getListSize() {
    return listSize;
}

void pushAvFrame(AVFrame *avFrame) {
    if (!avFrame) {
        return;
    }
    if (tail->value == NULL) {
        tail->value = avFrame;
    } else {
        ListEntity *listEntity = malloc(sizeof(ListEntity));
        listEntity->value = avFrame;
        tail->next = listEntity;
        tail = listEntity;
    }
    listSize++;

};

AVFrame *popAvFrame() {
    AVFrame *value = head->value;
    if (!value) {
        return NULL;
    }
    listSize--;
    if (!head->next) {
        return value;
    }
    ListEntity *newHead = head->next;
    free(head);
    head = newHead;
    return value;
}


