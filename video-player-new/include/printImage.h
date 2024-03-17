
#ifndef _PRINTIMAGE_H_
#define _PRINTIMAGE_H_
#include <stdlib.h>
#include "./video_decoder.h"
#include <stdio.h>
#include <unistd.h>
// 传递参数的结构体
typedef struct parameters{
    char* fileName;
    int size;
    int stride;
}Para;
// 定义获取视频帧的函数
void getFrameAndPrint(char* fileName,int size,int stride,int ifColor);

// 定义一个判断函数,判断得到的视频帧的处理方式
//void judgeFrame(char* fileName,int size,int stride,int ifColor);
// 打印灰度图

void printGrayImage(Frame frame);
// 打印rgb图像
void printRGBImage(Frame frame);
// 利用平均池化技术
void resizeByAverage(Frame* frame,int size,int stride);
// 利用最大值池化技术
void resizeByMax(Frame* frame,int size,int stride);
// 完成连续打印
void printInRow(char* fileName,int size,int stride,int ifColor);
// 视频处理线程
void* saveFrame(void* arg);
// 打印线程
void* printImage(void* arg);
// 初始化
void bufInit();
// 写入文件
void writeIntoTxt(Frame frame);
// 读取打印视频
void printVideoByTxt(char* fileName,double fps);
#endif 