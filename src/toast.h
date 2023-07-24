#ifndef TOAST_H
#define TOAST_H

#include <stdint.h>

#define TOAST_STRING_MAX 64

void ToastInit(void);
void ToastAddAchieved(const char* achievement_name);
void ToastAddProgress(const char* achievement_name, int count, int goal);
void ToastOnPaint(void);

#endif
