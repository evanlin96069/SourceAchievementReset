#ifndef KEYVALUES_H
#define KEYVALUES_H

#include <stdint.h>

#include "vcall.h"

typedef abstract_class IKeyValuesSystem *IKeyValuesSystem;
typedef struct KeyValues KeyValues;

#define INVALID_KEY_SYMBOL (-1)
typedef int HKeySymbol;

abstract_class IKeyValuesSystem {
    void (*virtual RegisterSizeofKeyValues)(void *this, int size);
    void *(*virtual AllocKeyValuesMemory)(void *this, int size);
    void (*virtual FreeKeyValuesMemory)(void *this, void *pMem);
    HKeySymbol (*virtual GetSymbolForString)(void *this, const char *name,
                                             bool bCreate);
    const char *(*virtual GetStringForSymbol)(void *this, HKeySymbol symbol);
    void (*virtual AddKeyValuesToMemoryLeakList)(void *this, void *pMem,
                                                 HKeySymbol name);
    void (*virtual RemoveKeyValuesFromMemoryLeakList)(void *this, void *pMem);
};

IKeyValuesSystem *KeyValuesSystem(void);

enum types_t {
    TYPE_NONE = 0,
    TYPE_STRING,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_PTR,
    TYPE_WSTRING,
    TYPE_COLOR,
    TYPE_UINT64,
    TYPE_NUMTYPES,
};

struct KeyValues {
    int key_name;
    char *str_val;
    wchar_t *wstr_val;
    union {
        int val;
        float f_val;
        void *p_val;
        uint8_t color[4];
    };

    char data_type;
    char has_escapes;
    char unused[2];

    KeyValues *next;
    KeyValues *sub;
    KeyValues *chain;
};

KeyValues *KvFindKey(const KeyValues *kv, const char *key_name);
void KvFree(KeyValues *kv);

#endif
