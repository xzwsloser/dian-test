
#include "../include/printImage.h"
#include "../include/frameQueue.h"
#include <string.h>
#include <unistd.h>
const char* standard=" -.`+=@*!^ca#";           // 总共可以定义13个字符,前面12个每一个对应20,后面一个对应10个
Buffer* buf;  // 就是定义一个缓冲区
int flag=0;  // 记录视频帧录入状态
pthread_mutex_t mutex;  // 锁对象
char* bufferChar[256];
// 另外一个线程锁
double fps=1.0;  // 视频帧率
int ifStop=0;
// 初始化灰度值数组的函数
void initBufferChar(){
    for(int i=0;i<256;i++){
        char* buffer=(char*)malloc(sizeof(char)*30);
        sprintf(buffer,"\033[38;2;%i;%i;%im%c\033[0m",i,i,i,standard[i/20]);
        bufferChar[i]=buffer;
    }
}
// 定义一个修改ifStop的接口
int getifStop(){
    return ifStop;
}
void setifStop(int data){
    ifStop=data;
}
double getFps(){
    return fps;
}
void setFps(double data){
    fps=data;
}
// 初始化锁对象
void mutexInit(){
    pthread_mutex_init(&mutex,NULL);  // 对外提供接口,初始化线程锁对象
}
// 首先定义一个初始化函数
void bufInit(){
    
    buf=createBuffer();
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
        // for(int i=0;i<300;i++){
        //     frame=decoder_get_frame();
        // }
        if(size>1&&stride>1){
            // 池化操作
            resizeByMax(&frame,size,stride);
        }
        if(ifColor==0){
             printGrayImage(frame);
            
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
    // 循环打印->
    
    for(int y=0;y<h;y+=9){
       
        for(int x=0;x<w;x+=6){
           
        
            // 打印图像
            int index=y*l+x*3;
            unsigned int red=(int)frame.data[index];
            unsigned int green=(int)frame.data[index+1];
            unsigned int blue=(int)frame.data[index+2];
            // 此时的打印方法就是可以利用ascii码值打印,每20个数字分为一组,每一组对饮不同的ascii码表中的字符
            unsigned int gray=(int)((red*30+green*59+blue*11)/100);
            // 如何利用gray计算区间 0 20 40 -> 0 1 2 
          fputs(bufferChar[gray],stdout);
           
            
        }
        putchar('\n');
    }
    // 设置帧率
        usleep((int)(600000/fps)); // 微妙级别的 1s=1000000us
   // system("clear");
       if(ifStop==1){
        char ch;
        while(1){
            // 阻塞等待
            ch=getchar();
            if(ch=='c'){
                ifStop=0;
                break;
            }
            
        }
    }
   puts("\033[2J\033[H");  // 利用转义字符清屏并且把光标移动到左上角
 

    
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
     usleep((int)(600000/fps)); // 微妙级别的 1s=1000000us
       if(ifStop==1){
        char ch;
        while(1){
            // 阻塞等待
            ch=getchar();
            if(ch=='c'){
                ifStop=0;
                break;
            }
            
        }
    }
    puts("\033[2J\033[H");
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
          
            //printVideoByTxt("./video.txt",get_fps());
            if(ifColor==0){
                printGrayImage(frame);
                // 开启线程读取
                // 初始化锁
              
            
            } else if(ifColor==1){
                printRGBImage(frame);
            }
            for(int i=0;i<5;i++){
                 frame=decoder_get_frame();
                 if(frame.height==0&&frame.width==0){
                    break;
                 }

            }
                  
        }
       
    }
    // 开始打印
    
    decoder_close();

}
// 读取文件的形式打印动画

// 视频解码线程
void* fileDecord(void* arg){
    Para* para=(Para*)arg;
    // 初始化视频解码器
    int status=decoder_init(para->fileName);
    if(status==-1){
        printf("视频解码器打开失败\n");
        exit(1);
    }
    else{
        // 开始把所有的视频存放到buffer中
        Frame frame=decoder_get_frame();
        while(frame.height!=0&&frame.width!=0){
            // 插入元素
            // 处理图像
            if(para->size>=1&&para->stride>=1){
                resizeByMax(&frame,para->size,para->stride);
            }
            headInsert(buf,frame);
            // 连续跳两帧
            for(int i=0;i<5;i++){
                frame=decoder_get_frame();
                if(frame.height==0&&frame.width==0){
                    break;
                }
            }
        }
        // 插入完成
        flag=1;
        decoder_close();
        // 线程退出函数
        pthread_exit(NULL);
    }
}
// 视频处理线程
void* videoPrint(void* arg){
    // 这个函数的参数就是一个结构体
    // 这个线程的任务就是不断从buffer中取出元素完成打印
    int* ifColor=(int*)arg;
    while(!(buf->next==NULL&&flag==1)){
       // 等待一下 
        // 开始取出元素
        // 上锁
        pthread_mutex_lock(&mutex);
        while(buf->next==NULL){
            if(buf->next==NULL&&flag==1){
                exit(0);
            }
        }  
        if(buf->next==NULL&&flag==1){
            break;
        }
        Frame frame=tailDelete(buf);
        if(frame.data==NULL){
            continue;
        }
        if(*ifColor==0){
            printGrayImage(frame);
        }
        else if(*ifColor==1){
            printRGBImage(frame);
        }
        pthread_mutex_unlock(&mutex);
    }
    // 线程退出函数
    pthread_exit(NULL);
}

