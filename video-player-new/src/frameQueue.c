#include "../include/frameQueue.h"
#include <unistd.h>
// 首先定义插入元素的方法,这里利用头插尾删的方法
Buffer* createBuffer(){
    Buffer* buf=(Buffer*)malloc(sizeof(Buffer));
    buf->next=NULL;
    buf->frame.data=NULL;
    buf->frame.height=0;
    buf->frame.linesize=0;
    buf->frame.width=0;
    return buf;
}
// 头插法插入元素
void headInsert(Buffer* buf,Frame frame){
    Buffer* pnew=(Buffer*)malloc(sizeof(Buffer));
    pnew->frame=frame;
    pnew->next=buf->next;
    buf->next=pnew;
}
// 尾删法
Frame tailDelete(Buffer* buf){
    // 首先找到尾节点
   if(buf->next==NULL){
        Frame frame;
        frame.data=NULL;
        frame.height=0;
        frame.linesize=0;
        frame.width=0;
        return frame;
   }
    Buffer* prun=buf;
    while(prun->next->next!=NULL){
        prun=prun->next;
    }
    Buffer* temp=prun->next;
    prun->next=NULL;
    return temp->frame;
}