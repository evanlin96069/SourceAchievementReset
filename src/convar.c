#include "convar.h"

#include <stdarg.h>
#include <string.h>

#include "dbg.h"
#include "interfaces.h"
#include "memalloc.h"

DECL_IFUNC(PRIVATE, int, icvar, AllocateDLLIdentifier, 5);
DECL_IFUNC(PRIVATE, void, icvar, RegisterConCommand, 6, ConCommandBase*);
DECL_IFUNC(PRIVATE, void, icvar, UnregisterConCommands, 8, int);
DECL_IFUNC(PUBLIC, ConVar*, icvar, FindVar, 12, const char*);
DECL_IFUNC(PUBLIC, ConCommand*, icvar, FindCommand, 14, const char*);

static const int vtidx_ConsoleColorPrintf = 23;

ConVarVTable vtable_ConVar = {0};
void* vtable_IConVar[ICONVAR_VTABLE_SIZE] = {0};
void* vtable_ConCommand[CONCOMMAND_VTABLE_SIZE] = {0};

static const int vtidx_GetDLLIdentifier = 7;

DECL_VFUNC(PRIVATE, void, InternalSetValue, 10, const char*);
DECL_VFUNC(PRIVATE, void, InternalSetFloatValue, 11, float);
DECL_VFUNC(PRIVATE, void, InternalSetIntValue, 12, int);

void SetValue(ConVar* this, const char* value) {
    InternalSetValue(this->parent, value);
}

void SetFloatValue(ConVar* this, float value) {
    InternalSetFloatValue(this->parent, value);
}

void SetIntValue(ConVar* this, int value) {
    InternalSetIntValue(this->parent, value);
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

static int virtual GetDLLIdentifier(ConCommandBase* this) {
    return DLL_identifier;
}

static bool Load(void) {
    icvar = engine_factory(CVAR_INTERFACE_VERSION, NULL);
    if (!icvar) {
        Warning("Failed to get ICvar interface.\n");
        return false;
    }

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

static void Unload(void) {
    UnregisterConCommands(DLL_identifier);

    _ConVar* curr = cvar_list.next;
    while (curr) {
        Free(curr->cvar.str_val);
        curr = curr->next;
    }
}

Module cvar_module = {
    .Load = &Load,
    .Unload = &Unload,
};
