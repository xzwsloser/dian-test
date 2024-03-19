#include "../include/kbhit.h"
#include "../include/printImage.h" 
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
    else if(ch=='s'){
        // 减速
        setFps(getFps()*0.8);
    }
    else if(ch==' '){
        // 暂停程序
        setifStop(1);
    }
   
    else if(ch=='q'){
        printf("\033[2J\033[H");
        break;
    }
    
   }
        exit(0); // 退出程序
   pthread_exit(NULL);
   
}