#include "keyvalues.h"

KeyValues* KvFindKey(const KeyValues* kv, const char* key_name) {
    if (!kv)
        return NULL;

    IKeyValuesSystem* kvs = KeyValuesSystem();
    HKeySymbol sym = (*kvs)->GetSymbolForString(kvs, key_name, false);
    if (sym == INVALID_KEY_SYMBOL)
        return NULL;

    for (KeyValues* dat = kv->sub; dat; dat = dat->next) {
        if (dat->key_name == sym)
            return dat;
    }

    return NULL;
}

void KvFree(KeyValues* kv) {
    IKeyValuesSystem* kvs = KeyValuesSystem();
    while (kv) {
        KvFree(kv->sub);
        KeyValues* next = kv->next;
        (*kvs)->FreeKeyValuesMemory(kvs, kv->str_val);
        (*kvs)->FreeKeyValuesMemory(kvs, kv->wstr_val);
        (*kvs)->FreeKeyValuesMemory(kvs, kv);
        kv = next;
    }
}
