#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>  
#include<string.h>
#include<pthread.h>
#include "../include/printImage.h"
#include "../include/parseParameters.h"
/**
 *  多线程没有成功,没有优化,没有暂停和加速
 *  -a选项播放视频
*/
int main(int argc,char* argv[]){
    // 定义一个默认的读取文件
    char* fileName="../video/bad_apple.mp4";
    // 利用循环读取的方式解析命令行参数
    int i=1;
    // 为了命令可以不分顺序,可以做一个参数汇总,最后集中传递给judge函数
    int ifColor=0; // 是否有颜色
    int rsize[2]={0,0}; // 窗口大小和步长
    int ifAction=0;
    while(argv[i]!=NULL){
        // 开始解析命令
        char *str=argv[i];
        if(strcmp(str,"-h")==0||strcmp(str,"--help")==0){
            // 提示帮助文档
           isHelp();
            
            i++;
            break;
        } else if(strcmp(str,"-v")==0||strcmp(str,"--version")==0){
            // 版本号
          
           isVersion();
            i++;
            break;
        } else if(strcmp(str,"-r")==0||strcmp(str,"--resize")==0){
            // 向后面遍历两个变量
            if(argv[i+1]!=NULL&&argv[i+2]!=NULL){
                // 直接获取参数
                rsize[0]=(int)argv[i+1][0]-48;
                rsize[1]=(int)argv[i+2][0]-48;
                // 之后就可以把参数传递给打印函数
                i+=3;
            }
            else{
                printf("请输入窗口的大小和采样的步长\n");
                break;
            }
        } else if(strcmp(str,"-c")==0||strcmp(str,"--color")==0){
            // 执行会话rgb图像的函数,这里还是可以把参数传递给打印函数
            ifColor=1;
            i++;
        } else if(strcmp(str,"-f")==0||strcmp(str,"--file")==0){
            // 向后面遍历一个索引
            if(argv[i+1]!=NULL){
                // 就可以把文件名传递给打印函数
                fileName=argv[i+1];
                i+=2;
            }
            else{

                // 参数的传递
                i++;
            }
        }
         else if(strcmp(str,"-a")==0||strcmp(str,"--action")==0){
                 ifAction=1;
                 i++;
        }
         else{
            printf("输出有误,请重新输入\n");
            exit(1);
        }
    
    }
    // 参数的汇总
    if(ifAction==1){
        printInRow(fileName,rsize[0],rsize[1],ifColor);
    } else if(ifAction==0){
        getFrameAndPrint(fileName,rsize[0],rsize[1],ifColor);
    }
    // 线程的开启
    // 打开读入视频帧的线程
    //首先构造结构体
    // Para* para=(Para*)malloc(sizeof(Para));
    // para->fileName=fileName;
    // para->size=rsize[0];
    // para->stride=rsize[1];
    // // 初始化缓存区
    // bufInit();
    // // 创建线程
//    // pthread_t dealVideo;  //视频提取线程
    // pthread_t printVideo; // 视频打印线程
    // //pthread_create(&dealVideo,NULL,saveFrame,para);
    // saveFrame(para);
    // sleep(2);
    // pthread_create(&printVideo,NULL,printImage,&ifColor);

    // // 线程退出函数
    // pthread_exit(NULL);
    
}
