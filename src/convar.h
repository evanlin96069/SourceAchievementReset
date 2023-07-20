#ifndef CONVAR_H
#define CONVAR_H

#include <stdint.h>

#include "vcall.h"

#define CVAR_INTERFACE_VERSION "VEngineCvar004"

typedef void* ICvar;

typedef struct CCommand CCommand;
typedef struct ConCommandBase ConCommandBase;
typedef struct ConCommand ConCommand;
typedef abstract_class IConVar* IConVar;
typedef struct ConVar ConVar;

// clang-format off

// ConVars and ConCommands flags
#define FCVAR_NONE                    0         // The default, no flags at all
#define FCVAR_UNREGISTERED            (1 << 0)
#define FCVAR_DEVELOPMENTONLY         (1 << 1)
#define FCVAR_GAMEDLL                 (1 << 2)  // Defined by the game DLL
#define FCVAR_CLIENTDLL               (1 << 3)  // Defined by the client DLL
#define FCVAR_HIDDEN                  (1 << 4)  // Hidden in find or autocomplete

// ConVar only
#define FCVAR_PROTECTED               (1 << 5)  // Don't send to client
#define FCVAR_SPONLY                  (1 << 6)
#define FCVAR_ARCHIVE                 (1 << 7)  // Saved to config
#define FCVAR_NOTIFY                  (1 << 8)  // Announce changes in game chat
#define FCVAR_USERINFO                (1 << 9)
#define FCVAR_CHEAT                   (1 << 14) // Only useable in singleplayer & sv_cheats

#define FCVAR_PRINTABLEONLY           (1 << 10) // Cannot contain unprintable characters
#define FCVAR_UNLOGGED                (1 << 11)
#define FCVAR_NEVER_AS_STRING         (1 << 12) // Never try to print that cvar
#define FCVAR_REPLICATED              (1 << 13) // Server setting enforced on clients
#define FCVAR_DEMO                    (1 << 16) // Record this cvar when starting a demo file
#define FCVAR_DONTRECORD              (1 << 17) // Don't record these command in demofiles
#define FCVAR_RELOAD_MATERIALS        (1 << 20) // Reload materials on change
#define FCVAR_RELOAD_TEXTURES         (1 << 21) // Reload textures on change
#define FCVAR_NOT_CONNECTED           (1 << 22) // Cannot be changed while connected
#define FCVAR_MATERIAL_SYSTEM_THREAD  (1 << 23)
#define FCVAR_ARCHIVE_XBOX            (1 << 24)
#define FCVAR_ACCESSIBLE_FROM_THREADS (1 << 25)
#define FCVAR_SERVER_CAN_EXECUTE      (1 << 28)
#define FCVAR_SERVER_CANNOT_QUERY     (1 << 29)
#define FCVAR_CLIENTCMD_CAN_EXECUTE   (1 << 30)

// clang-format on

#define COMMAND_MAX_ARGC 64
#define COMMAND_MAX_LENGTH 512

struct CCommand {
    int argc;
    int argv_0_size;
    char args_buffer[COMMAND_MAX_LENGTH];
    char argv_buffer[COMMAND_MAX_LENGTH];
    const char* argv[COMMAND_MAX_ARGC];
};

struct ConCommandBase {
    void** vtable;
    ConCommandBase* next;
    bool registered;
    const char* name;
    const char* help_string;
    int flags;
};

typedef void (*FnCommandCallbackVoid_t)(void);
typedef void (*FnCommandCallback_t)(const CCommand*);

#define COMMAND_COMPLETION_MAXITEMS 64
#define COMMAND_COMPLETION_ITEM_LENGTH 64

typedef int (*FnCommandCompletionCallback)(
    const char* partial,
    char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]);

struct ConCommand {
    ConCommandBase base;
    union {
        FnCommandCallbackVoid_t callback_v1;
        FnCommandCallback_t callback;
        /* ICommandCallback* */ void* icallback;
    };
    union {
        FnCommandCompletionCallback completion_callback;
        /* ICommandCompletionCallback* */ void* completion_icallback;
    };
    bool has_completion_callback : 1;
    bool using_new_callback : 1;
    bool using_icallback : 1;
};

abstract_class IConVar {
    // Reverse order because of overload ordering
    void (*virtual SetValue_i)(void* this, int nValue);
    void (*virtual SetValue_f)(void* this, float flValue);
    void (*virtual SetValue)(void* this, const char* pValue);

    const char* (*virtual GetName)(void* this);
    bool (*virtual IsFlagSet)(void* this, int nFlag);
};

typedef void (*FnChangeCallback_t)(IConVar* var, const char* old_val,
                                   float f_old_val);

struct ConVar {
    ConCommandBase base1;
    IConVar base2;
    struct ConVar* parent;
    const char* default_val;
    char* str_val;
    int str_len;
    float f_val;
    int val;
    bool has_min;
    float min_val;
    bool has_max;
    float max_val;
    FnChangeCallback_t change_callback;
};

// An internal ConVar list for deallocating all of them when the plugin unload
typedef struct _ConVar {
    ConVar cvar;
    struct _ConVar* next;
} _ConVar;

#define CONVAR_VTABLE_SIZE 16
typedef struct ConVarVTable {
    void* rtti;
    void* vtable[CONVAR_VTABLE_SIZE];
} ConVarVTable;

extern ConVarVTable vtable_ConVar;

#define ICONVAR_VTABLE_SIZE 5
extern void* vtable_IConVar[ICONVAR_VTABLE_SIZE];

#define CONCOMMAND_VTABLE_SIZE 13
extern void* vtable_ConCommand[CONCOMMAND_VTABLE_SIZE];

#define _CONVAR(name_, desc, value, hasmin_, min, hasmax_, max, flags_)        \
    static _ConVar _cvar_##name_ = {                                           \
        .cvar =                                                                \
            {.base1 = {.vtable = (void**)&vtable_ConVar.vtable,                \
                       .name = "" #name_,                                      \
                       .help_string = "" desc,                                 \
                       .flags = (flags_)},                                     \
             .base2 = (IConVar)vtable_IConVar,                                 \
             .parent = &_cvar_##name_.cvar,                                    \
             .default_val =                                                    \
                 _Generic(value, char*: value, int: #value, double: #value),   \
             .str_len = _Generic(value,                                        \
             char*: sizeof(value), /*NOLINT*/                                  \
             default: sizeof(#value)),                                         \
             .f_val = _Generic(value, char*: 0, int: value, double: value),    \
             .val = _Generic(value, char*: 0, int: value, double: (int)value), \
             .has_min = hasmin_,                                               \
             .min_val = (min),                                                 \
             .has_max = hasmax_,                                               \
             .max_val = (max)},                                                \
    };                                                                         \
    ConVar* name_ = &_cvar_##name_.cvar

/* Defines a console variable with no min/max values. */
#define CONVAR(name, desc, value, flags) \
    _CONVAR(name, desc, value, false, 0, false, 0, flags)

/* Defines a console variable with a given mininum numeric value. */
#define CONVAR_MIN(name, desc, value, min, flags) \
    _CONVAR(name, desc, value, true, min, false, 0, flags)

/* Defines a console variable with a given maximum numeric value. */
#define CONVAR_MAX(name, desc, value, max, flags) \
    _CONVAR(name, desc, value, false, 0, true, max, flags)

/* Defines a console variable in the given numeric value range. */
#define CONVAR_MINMAX(name, desc, value, min, max, flags) \
    _CONVAR(name, desc, value, true, min, true, max, flags)

#define _DEF_CCMD(varname, name_, desc, func, flags_) \
    static ConCommand _ccmd_##varname = {             \
        .base = {.vtable = vtable_ConCommand,         \
                 .name = "" #name_,                   \
                 .help_string = "" desc,              \
                 .flags = (flags_)},                  \
        .callback = &func,                            \
        .using_new_callback = true,                   \
    };                                                \
    ConCommand* varname = (ConCommand*)&_ccmd_##varname

/* Defines a command with a given function as its handler. */
#define DEF_CCMD(name, desc, func, flags) \
    _DEF_CCMD(name, name, desc, func, flags)

/*
 * Defines two complementary +- commands, with PLUS_ and MINUS_ prefixes on
 * their C names.
 */
#define DEF_CCMD_PLUSMINUS(name, descplus, fplus, descminus, fminus, flags) \
    _DEF_CCMD(PLUS_##name, "+" name, descplus, fplus, flags);               \
    _DEF_CCMD(MINUS_##name, "-" name, descminus, fminus, flags)

/*
 * Defines a console command with the handler function body immediately
 * following the macro (like in Source itself).
 */
#define CON_COMMAND(name, desc, flags)                \
    static void _cmdf_##name(const CCommand* args);   \
    _DEF_CCMD(name, name, desc, _cmdf_##name, flags); \
    static void _cmdf_##name(const CCommand* args) /* { body here } */

// Cvar Interface
ConVar* FindVar(const char* name);
ConCommand* FindCommand(const char* name);

// ConVar
void SetValue(ConVar* this, const char* value);
void SetFloatValue(ConVar* this, float value);
void SetIntValue(ConVar* this, int value);

// Register
void InitConVar(ConVar* cvar);
void InitCommand(ConCommand* cmd);

// Color print
typedef struct Color {
    union {
        struct {
            uint8_t r, g, b, a;
        };
        uint32_t val;
        uint8_t bytes[4];
    };
} Color;

extern const int vtidx_ConsoleColorPrintf;
typedef void (*ConsoleColorPrintf_func)(void*, const Color*, const char*, ...);
#define ConsoleColorPrintf(clr, fmt, ...) \
    VFUNC(icvar, ConsoleColorPrintf)(icvar, clr, fmt __VA_OPT__(, ) __VA_ARGS__)

// Module
bool LoadCvarModule(void);
void UnloadCvarModule(void);

#endif
