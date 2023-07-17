#ifndef DBG_H
#define DBG_H

void Msg(const char* fmt, ...)
    __attribute__((format(printf, 1, 2))) __asm__("_Msg");
void Warning(const char* fmt, ...)
    __attribute__((format(printf, 1, 2))) __asm__("_Warning");

#endif
