#ifndef _FRAMEQUEUE_H_
#define _FRAMEQUEUE_H_
#include<stdio.h>
#include <stdlib.h>
#include "./video_decoder.h"
#include <pthread.h>
// 这里利用队列作为缓存
typedef struct buffer{
    Frame frame;
    struct buffer* next;
}Buffer;
// 传递给线程的参数


// 首先定义插入元素的方法,这里利用头插尾删的方法
Buffer* createBuffer();
  
// 头插法插入元素
void headInsert(Buffer* buf,Frame frame);

// 尾删法
Frame tailDelete(Buffer* buf);


#endif