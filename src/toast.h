#ifndef TOAST_H
#define TOAST_H

#include <stdint.h>

#define TOAST_STRING_MAX 64

void ToastInit(void);
void ToastAdd(const char* achievement_name,
              const wchar_t title[TOAST_STRING_MAX],
              const wchar_t desc[TOAST_STRING_MAX]);
void ToastOnPaint(void);

#endif
