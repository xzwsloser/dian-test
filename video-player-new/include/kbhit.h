#ifndef _KBHIT_H_
#define _KBHIT_H_
#include <stdio.h>
#include <termios.h>
#include <term.h>
#include <unistd.h>
#include <asm-generic/fcntl.h>
#include <pthread.h>
void*  kbhit();
void* readChar();
#endif