# Dian团队春招学习记录
## 总结
还没有解决的问题:
1. 视频帧率过快,rsize之后的视频帧大小过大还是会出现闪屏现象
2. rsize调节的范围有限
其他的功能基本实现
## 运行方式
```bash
cd output
./player -f ../video/bad_apple.mp4 -r 2 2 
```
## Level 0-1
之前就有学习过git,系统复习了一遍git中的常见指令
## Level 0-2
参数解析,网上查到可以使用unstid头文件下的getopt函数解析参数,但是利用这个函数解析参数时-r 之后的两个参数记录比较困难,改为直接解析argv[i],一个一个参数地判断,最后统计参数的值进行相应操作,代码如下:
```c
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
```
## Level 0-3
安装相关的库,使用ubuntu中的apt或者centos中的yum即可,如果安装失败可以更新仓库 sudo apt updata
## Level 1-1
使用到的ANSI值:
1. 前景色: \033[38;red;green;bluem内容\033[0m
2. 背景色: \033[48;2;red;green;bluem内容\033[0m
3. 清屏并且把光标移动到开头位置: "\033[2J\033[H"
存在的问题:   
1. 如果每一次只打印一个像素点,由于像素点的大小和方块的大小相差较多,所以可以每个几行几列打印一次   
2. 可以使用字符画的形式打印视频帧,显示灰度差异
代码演示:
```c
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
            printf("\033[38;2;%u;%u;%um%c\033[0m",gray,gray,gray,standard[gray/20]);
            
            
        }
        printf("\n");
    }
    // 设置帧率
        usleep(100000); // 微妙级别的 1s=1000000us
   // system("clear");
  
   puts("\033[2J\033[H");  // 利用转义字符清屏并且把光标移动到左上角   
}
```
## Level1-2
利用卷积池化的技术完成视频帧图片的缩小
1. 卷积: 其实就是对于一块区域里面的像素点扫描求和
2. 池化: 取其中的平均值或者最大值作为这一块区域的代表
问题: 注意池化之后frame大小的改变
代码实现:(以最大值池化为例)
```c
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
```
## Level 1-3
1. 最先开始的做法就是利用循环不断打印图片,但是这样的效果非常差
2. 调节帧率利用usleep函数每打印一张图片之后线程休眠一段事件
3. 利用上面的ASNI清屏
## Level 2-1
1. 学习了C语言多线程的知识,学习了创建线程,插入线程,退出线程的语法
2. 了解了多个线程操作同一个全局变量时线程安全的问题,可以利用锁把操作共享变量所在的代码块锁起来
3. 这里利用队列这种数据结果作为缓存,因为它的先进先出的特性保证了视频帧的顺序
4. 可以利用结构体的方式传递参数
5. 代码演示(演示取出缓存和开启线程的代码):
取出缓存:
```c
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
        while(buf->next==NULL){};  
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
```
开启线程,初始化锁:
```c
  // 初始化线程锁对象
    mutexInit();
 
    // 开启线程
    pthread_t tid1;  // 视频解码线程
    pthread_t tid2;
   
    // 参数的转化
    Para* para=(Para*)malloc(sizeof(Para));
    para->fileName=fileName;
    para->size=rsize[0];
    para->stride=rsize[1];
    pthread_create(&tid1,NULL,fileDecord,para);
   
        pthread_create(&tid2,NULL,videoPrint,&ifColor);
    
    // 插入线程
    pthread_join(tid1,NULL);
   
    pthread_join(tid2,NULL);
```
## Level 2-2
可以每隔两帧取一帧进行打印
## Level 3-1
1. 实现视频的暂停,可以实现windows中的kbhit函数异步监听键盘事件,另外自己写一个readChar函数不断把stdin中的字符读出来进行相应处理
2. 利用多线程的方式,一条线程用于监听(就是kbhit函数),另外一个线程用于读取输入的字符并且进行相应的处理
3. 通过操作全局变量实现暂停
## Level 3-2
实现方式和上面差不多   
代码实现
```c
void* kbhit()  
{  
   while(1){
        struct termios oldt, newt;  
    int ch;  
    int oldf;  
    tcgetattr(STDIN_FILENO, &oldt);  
    newt = oldt;  
    newt.c_lflag &= ~(ICANON | ECHO);  
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  
    
    /* set the nonblock */
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);  
    ch = getchar();  
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0));  
    if (ch != 'q')  
    {  
        ungetc(ch, stdin);  /* back the ch to stdin */
       
    } else{
        
        ungetc(ch, stdin);  /* back the ch to stdin */
        break;
    } 
   
   }
   pthread_exit(NULL);
 
}  
void* readChar(){
   while(1){
     char ch=getchar();
    if(ch=='d'){
        // 加速
        setFps(getFps()*1.2);
    }
    else if(ch==' '){
        // 暂停程序
        setifStop(1);
    }
   
    else if(ch=='q'){
        exit(0); // 退出程序
    }
    
   }
   pthread_exit(NULL);
   
}
```





