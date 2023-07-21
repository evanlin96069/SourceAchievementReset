#ifndef TOAST_H
#define TOAST_H

#include <stdint.h>

#define TOAST_STRING_MAX 64

void ToastInit(void);
void ToastAdd(const wchar_t title[TOAST_STRING_MAX],
              const wchar_t desc1[TOAST_STRING_MAX],
              const wchar_t desc2[TOAST_STRING_MAX]);
void ToastOnPaint(void);

#endif
