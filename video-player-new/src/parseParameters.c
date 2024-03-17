#include "../include/parseParameters.h"
void isVersion(){
    printf("Dian-player v1.2");
}

void isHelp(){
    printf("player: [-v][-h][-f filename][-c][-r windowSize length]\n");
    printf("选项:\n");
    printf("-h --help         用于显示命令的可选项和选项参数\n");
    printf("-v --version      用于显示命令的版本号\n");
    printf("-c --color        调整降低采样的窗口大小和步长大小,具体参数需要指定\n");
    printf("-f --file         选择需要播放的视频文件,具体参数需要指定\n");
}

