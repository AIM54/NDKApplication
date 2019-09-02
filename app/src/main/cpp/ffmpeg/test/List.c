//
// Created by Administrator on 2019/9/2.
//
#include "List.h"

List MakeEmpty(List L) {
    if (NULL == L)
        return L;
    Position p = First(L);
    Position tmp = p;
    while (NULL != p) {
        tmp = p->next;
        free(p);
        p = tmp;
    }
    L->next = NULL;
    return L;
}

int IsEmpty(List L) {
    return First(L) == NULL;
}

int IsLast(Position P, List L) {
    return P->next == NULL;
}

List CreateEmptyList() {
    List L = (List) malloc(sizeof(struct node));
    if (NULL == L) return NULL;
    L->next = NULL;
    return L;
}

Position Find(Element X, List L) {
    if (NULL == L) return NULL;
    Position p = First(L);
    while (NULL != p && p->data != X)
        p = p->next;
    return p;
}

void Delete(Element X, List L) {
    if (NULL == L) return;
    Position prev = FindPrevious(X, L);
    if (!IsLast(prev, L)) {
        Position tmp = prev->next;
        prev->next = tmp->next;
        free(tmp);
    }
}

Position FindPrevious(Element X, List L)//查找失败则返回链表的最后一个元素
{
    if (NULL == L) return NULL;
    Position prev = Header(L);
    while (NULL != prev->next && X != prev->next->data)
        prev = prev->next;
    return prev;
}

void Insert(Element X, Position P, List L)//在P的后面插入新元素X
{
    if (NULL == L) return;
    Position tmp = (Position) malloc(sizeof(struct node));
    if (NULL == tmp) return;
    tmp->data = X;
    tmp->next = P->next;
    P->next = tmp;
}

void PushBack(Element X, List L) {
    if (NULL == L) return;
    Position pos = Header(L);
    while (NULL != pos->next)
        pos = pos->next;
    Position tmp = (Position) malloc(sizeof(struct node));
    if (NULL == tmp) return;
    tmp->data = X;
    tmp->next = NULL;
    pos->next = tmp;
}

void PushFront(Element X, List L) {
    if (NULL == L) return;
    Position pos = L->next;
    Position tmp = (Position) malloc(sizeof(struct node));
    if (NULL == tmp) return;
    tmp->data = X;
    tmp->next = pos;
    L->next = tmp;
}

void DeleteList(List L) {
    MakeEmpty(L);
    free(L);
}

Position Header(List L) {
    return L;
}

Position First(List L) {
    if (NULL == L) return NULL;
    return L->next;
}

Position Advance(Position P, List L)//得到P的前一个位置
{
    if (NULL == L) return NULL;
    Position prev = Header(L);
    while (NULL != prev && prev->next != P)
        prev = prev->next;
    return prev;
}

Element Retrieve(Position P) {
    return P->data;
}
