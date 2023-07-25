#ifndef MODULE_H
#define MODULE_H

#include <stdbool.h>

typedef void (*ModuleCallback_func)(void);

typedef enum ModuleSignalType {
    MODULE_ON_PAINT,
    MODULE_SIGNAL_COUNT,
} ModuleSignalType;

typedef struct Module {
    bool is_loaded;
    bool (*Load)(void);
    void (*Unload)(void);
    ModuleCallback_func callbakcs[MODULE_SIGNAL_COUNT];
} Module;

bool LoadModules(void);
void UnloadModules(void);
void SingalModules(ModuleSignalType type);

#endif
