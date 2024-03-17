
#include "../include/printImage.h"
#include "../include/frameQueue.h"
#include <string.h>
#include <unistd.h>
const char* standard=" .-^*!+=&@#$h";           // 总共可以定义13个字符,前面12个每一个对应20,后面一个对应10个
Buffer* buf=NULL;
int flag=0;  // 记录视频帧录入状态
// 首先定义一个初始化函数
void bufInit(){
    if(buf==NULL){
        // 初始化
        buf=createBuffer();
    }
    else{
        return ;
    }
}
// 视频帧写入到文件中
void writeIntoTxt(Frame frame){
    int w=frame.width;
    int h=frame.height;
    int l=frame.linesize;
    FILE* file=fopen("./video.txt","a");
    for(int y=0;y<h;y+=9){
        for(int x=0;x<w;x+=4){
            // 打印图像
            int index=y*l+x*3;
            unsigned int red=(int)frame.data[index];
            unsigned int green=(int)frame.data[index+1];
            unsigned int blue=(int)frame.data[index+2];
            // 此时的打印方法就是可以利用ascii码值打印,每20个数字分为一组,每一组对饮不同的ascii码表中的字符
            unsigned int gray=(int)((red*30+green*59+blue*11)/100);
            char buf[200];
            fprintf(file,"\033[38;2;%u;%u;%um%c\033[0m",gray,gray,gray,standard[gray/20]);
        }
            fprintf(file,"\n");
    }
        fprintf(file,"\033[2J\033[Hy");
        fclose(file);
}
// 定义获取视频帧的函数
void getFrameAndPrint(char* fileName,int size,int stride,int ifColor){
    // 首先初始化视频解码器
    int status=decoder_init(fileName);
    if(status==-1){
        printf("视频解析器打开失败\n");
        exit(1); // 退出
    } else{
        // 首先获取视频帧
        Frame frame=decoder_get_frame();
        for(int i=0;i<300;i++){
            frame=decoder_get_frame();
        }
        if(size>1&&stride>1){
            // 池化操作
            resizeByMax(&frame,size,stride);
        }
        if(ifColor==0){
            // printGrayImage(frame);
            writeIntoTxt(frame);
        }
        else if(ifColor==1){
            // 打印彩色图像
            printRGBImage(frame);
        }
        decoder_close();

    }
}

// level1-1 首先打印灰度图像
void printGrayImage(Frame frame){
   
    int w=frame.width;
    int h=frame.height;
    int l=frame.linesize;
    // 循环打印
  
    for(int y=0;y<h;y+=9){
        for(int x=0;x<w;x+=4){
   
            // 打印图像
            int index=y*l+x*3;
            unsigned int red=(int)frame.data[index];
            unsigned int green=(int)frame.data[index+1];
            unsigned int blue=(int)frame.data[index+2];
            // 此时的打印方法就是可以利用ascii码值打印,每20个数字分为一组,每一组对饮不同的ascii码表中的字符
            unsigned int gray=(int)((red*30+green*59+blue*11)/100);
            // 如何利用gray计算区间 0 20 40 -> 0 1 2 
   
            int num=gray/20;
            //char* buf="\033[38;2;0;0;0m%c\033[0m";
            // char buffer[100];
            // sprintf(buffer,"\033[38;2;%u;%u;%um%c\033[0m",gray,gray,gray,standard[num]);
            // puts(buffer);
            printf("\033[38;2;%u;%u;%um%c\033[0m",gray,gray,gray,standard[num]);
   
        }
        printf("\n");
    }
    // 设置帧率
        usleep(500000); // 微妙级别的 1s=1000000us
   // system("clear");
   /**
    * 优化: 利用双缓冲法优化
   */
   printf("\033[2J\033[H");  // 利用转义字符清屏并且把光标移动到左上角
   
    
}
// level1-1 打印rgb图像
void printRGBImage(Frame frame){
    int w=frame.width;
    int h=frame.height;
    int l=frame.linesize;
    for(int y=0;y<h;y+=9){
        for(int x=0;x<w;x+=6){
            int index=y*l+x*3;
            unsigned int red=(int)frame.data[index];
            unsigned int green=(int)frame.data[index+1];
            unsigned int blue=(int)frame.data[index+2];
            printf("\033[48;2;%u;%u;%um \033[0m",red,green,blue);
        }
        printf("\n");
    }    
}
// level1-2 平均池化
// 就是取一个区间里面的平均值
/**
 *  size为取样窗口的大小,stride为步长,新窗口的大小和步长有关
*/
void resizeByAverage(Frame* frame,int size,int stride){
        int w=frame->width;
        int h=frame->height;
        int l=frame->linesize;
        int newWidth=(w-size)/stride+1;
        int newHeigth=(h-size)/stride+1;
        unsigned char* data=(unsigned char*)malloc(sizeof(unsigned char)*(newHeigth*newWidth*3));
        int count=0;
        // 其实就是一个窗口的移动
        // y方向上面的步长要对应变化
        for(int y=0;y<h;y+=stride){
            for(int x=0;x<w;x+=stride){
                // 开始采样
                unsigned int red_sum=0;
                unsigned int green_sum=0;
                unsigned int blue_sum=0;
                for(int i=x;i<x+size;i++){
                    for(int j=y;j<y+size;j++){
                        int index=i*3+j*l;
                        red_sum+=frame->data[index];
                        green_sum+=frame->data[index+1];
                        blue_sum+=frame->data[index+2];
                    }   
                }
                // 求解平均值
                int num=size*size;
                data[count++]=red_sum/num;
                data[count++]=green_sum/num;
                data[count++]=blue_sum/num;
            }

        }
        // 最后重新赋值
        frame->data=data;
        frame->height=newHeigth;
        frame->width=newWidth;
        frame->linesize=3*newWidth;  // 注意池化之后的宽度和长度变化
    
}
// level1-2 最大值池化
void resizeByMax(Frame* frame,int size,int stride){
    int w=frame->width;
    int h=frame->height;
    int l=frame->linesize;
    int newWidth=(w-size)/stride+1;
    int newHeigth=(h-size)/stride+1;
    unsigned char* data=(unsigned char*)malloc(sizeof(unsigned char)*(newWidth*newHeigth*3));
    int count=0;
    for(int y=0;y<h;y+=stride){
        for(int x=0;x<w;x+=stride){
            unsigned int red_max=0;
            unsigned int green_max=0;
            unsigned int blue_max=0;
            for(int i=x;i<x+size;i++){
                for(int j=y;j<y+size;j++){
                    int index=j*l+i*3;
                    unsigned int red=(int)frame->data[index];
                    unsigned int green=(int)frame->data[index+1];
                    unsigned int blue=(int)frame->data[index+2];
                    if(red>red_max){
                    red_max=red;
                    }   
                    if(green>green_max){
                        green_max=green;
                    }
                    if(blue>blue_max){
                        blue_max=blue;
                    }
                }
               
                
            }
             data[count++]=red_max;
                data[count++]=green_max;
                data[count++]=blue_max;
        }
    }
    frame->data=data;
    frame->height=newHeigth;
    frame->width=newWidth;
    frame->linesize=newWidth*3;
}
// level 1-3 让图片动起来
void printInRow(char* fileName,int size,int stride,int ifColor){
    // 初始化解码器
    int status=decoder_init(fileName);
    if(status==-1){
        printf("视频解码器初始化失败\n");
        exit(1);
    } else{
        Frame frame=decoder_get_frame();
        while(frame.height!=0&&frame.width!=0){
            // 判断是否需要池化
            if(size>1&&stride>1){
                resizeByMax(&frame, size,stride);
            } 
            // 首先可以把字符全部读取到文件中
           // writeIntoTxt(frame);
            //printVideoByTxt("./video.txt",get_fps());
            if(ifColor==0){
                printGrayImage(frame);
            } else if(ifColor==1){
                printRGBImage(frame);
            }
            for(int i=0;i<2;i++){
                 frame=decoder_get_frame();
            }
                  
        }
    }
    // 开始打印
   printVideoByTxt("./video.txt",1);
    decoder_close();

}
// 视频解码线程的任务,就是利用头插法把视频帧放在buffer中
void* saveFrame(void* arg){
   
    Para* para=(Para*)arg;
  
    // 不断取出视频帧,加入到buf中,此时只用把这个看成一个任务就可以了
    int status=decoder_init(para->fileName);
   
    if(status==-1){
   
        printf("视频解码器打开失败\n");
        exit(1);
    }
    else{
        // 开始执行逻辑
    
        Frame frame=decoder_get_frame();
    
        while(frame.height!=0&&frame.width!=0){
    
            if(para->size>1&&para->stride>1){
    
                resizeByMax(&frame,para->size,para->stride);
   
            }
   
            headInsert(buf,frame);
    
            frame=decoder_get_frame();
   
            // 进行resize操作

        }
    }
        // 最后就可以把标志位置为1
        flag=1;
    // 放入完成销毁视频解码器
        decoder_close();
        return NULL;
}
// 图像处理线程,它的任务就是不断处理线程
void* printImage(void* arg){
    
    int* ifColor=(int*)arg;
    
    // 就是不断从buf中取出元素
    while(buf->next==NULL){
   

    }
    while(buf->next!=NULL){
   
        // 不断从中取出线程
        Frame frame=tailDelete(buf);
   
        if(*ifColor==0){
 
            // 打印灰度图
            printGrayImage(frame);
    
        } else if(*ifColor==1){
   
            // 打印彩色图片
            printRGBImage(frame);
   
        }
       
   
        // 就可以进行等待
        while(buf->next==NULL&&flag==0){
               
        }
    
    }
    return NULL;
}
// 读取文件的形式打印动画
void printVideoByTxt(char* fileName,double fps){
    // 一个字符读取
    FILE* file=fopen(fileName,"r+");
    rewind(file);
    if(file==NULL){
        printf("文件打开失败\n");
    }
    int ch;
    while((ch)=fgetc(file)!=EOF){
        if(ch=='y'){
            usleep(1000000/fps);
            continue;
        }
        printf("%c",ch);
    }
}
