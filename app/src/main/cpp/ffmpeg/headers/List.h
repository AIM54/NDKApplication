//
// Created by Administrator on 2019/9/2.
//

#ifndef NDKAPPLICATION_LIST_H
#define NDKAPPLICATION_LIST_H
#include <stdlib.h>
#define Element int
struct node
{
    Element data;
    struct node *next;
};
typedef struct node* PtrToNode;
typedef PtrToNode Position;
typedef Position List;
List MakeEmpty(List L);
int IsEmpty(List L);
int IsLast(Position P, List L);
List CreateEmptyList();
Position Find(Element X, List L);
void Delete(Element X, List L);
Position FindPrevious(Element X, List L);//查找失败则返回链表的最后一个元素
void Insert(Element X, Position P, List L);//在P的后面插入新元素X
void PushBack(Element X, List L);
void PushFront(Element X, List L);
void DeleteList(List L);
Position Header(List L);
Position First(List L);
Position Advance(Position P, List L);//得到P的前一个位置
Element Retrieve(Position P);
#endif //NDKAPPLICATION_LIST_H
