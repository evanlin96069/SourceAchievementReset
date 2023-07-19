#include "convar.h"

#include <stdarg.h>
#include <string.h>

#include "dbg.h"
#include "memalloc.h"
#include "plugin.h"

DECL_IFUNC(VCALL_PRIVATE, int, icvar, AllocateDLLIdentifier, 5);
DECL_IFUNC(VCALL_PRIVATE, void, icvar, RegisterConCommand, 6, ConCommandBase*);
DECL_IFUNC(VCALL_PRIVATE, void, icvar, UnregisterConCommands, 8, int);
DECL_IFUNC(VCALL_PUBLIC, ConVar*, icvar, FindVar, 12, const char*);
DECL_IFUNC(VCALL_PUBLIC, ConCommand*, icvar, FindCommand, 14, const char*);

const int vtidx_ConsoleColorPrintf = 23;

ConVarVTable vtable_ConVar = {0};
void* vtable_IConVar[ICONVAR_VTABLE_SIZE] = {0};
void* vtable_ConCommand[CONCOMMAND_VTABLE_SIZE] = {0};

const int vtidx_GetDLLIdentifier = 7;

DECL_VFUNC(VCALL_PRIVATE, void, InternalSetValue, 10, const char*);
DECL_VFUNC(VCALL_PRIVATE, void, InternalSetFloatValue, 11, float);
DECL_VFUNC(VCALL_PRIVATE, void, InternalSetIntValue, 12, int);

void SetValue(ConVar* thisptr, const char* value) {
    InternalSetValue(thisptr->parent, value);
}

void SetFloatValue(ConVar* thisptr, float value) {
    InternalSetFloatValue(thisptr->parent, value);
}

void SetIntValue(ConVar* thisptr, int value) {
    InternalSetIntValue(thisptr->parent, value);
}

static int DLL_identifier = 0;

static _ConVar cvar_list = {0};
void InitConVar(ConVar* cvar) {
    static _ConVar* curr = &cvar_list;

    cvar->str_val = Alloc(cvar->str_len);
    memcpy(cvar->str_val, cvar->default_val, cvar->str_len);
    RegisterConCommand((ConCommandBase*)cvar);

    // Should only register our cvar, so downcasing is fine.
    _ConVar* _cvar = (_ConVar*)cvar;
    curr->next = _cvar;
    curr = curr->next;
}

void InitCommand(ConCommand* cmd) { RegisterConCommand((ConCommandBase*)cmd); }

static int VCALLCONV GetDLLIdentifier(ConCommandBase* thisptr) {
    return DLL_identifier;
}

bool LoadCvarModule(void) {
    DLL_identifier = AllocateDLLIdentifier();

    /*
     * Instead of building our own vtables for ConVar, IConVar, and ConCommand,
     * we just copy vtables from a existing cvar and cmd. We also need to
     * replace the GetDLLIdentifier.
     */

    ConVar* cvar = FindVar("sv_gravity");
    if (cvar) {
        // ConVar vtable
        // rtti pointers are offset negatively from the vtable pointer.
        memcpy(&vtable_ConVar, cvar->base1.vtable - 1, sizeof(vtable_ConVar));

        // IConVar vtable
        // Although we don't need to replace anything, we still make a copy so
        // the vtable address will be known in compile time.
        memcpy(&vtable_IConVar, cvar->base2, sizeof(vtable_IConVar));
    }

    ConCommand* cmd = FindCommand("kill");
    if (cmd) {
        // ConCommand vtable
        memcpy(&vtable_ConCommand, cmd->base.vtable, sizeof(vtable_ConCommand));
    }

    if (!cvar || !cmd) {
        Warning("Failed to get cvar/cmd vtables.\n");
        return false;
    }

    // Replace GetDLLIdentifier
    vtable_ConVar.vtable[vtidx_GetDLLIdentifier] = &GetDLLIdentifier;
    vtable_ConCommand[vtidx_GetDLLIdentifier] = &GetDLLIdentifier;

    return true;
}

void UnloadCvarModule(void) {
    UnregisterConCommands(DLL_identifier);

    _ConVar* curr = cvar_list.next;
    while (curr) {
        Free(curr->cvar.str_val);
        curr = curr->next;
    }
}
