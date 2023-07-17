#include "example.h"

#include "convar.h"
#include "dbg.h"

// This is an example feature showing how to declare ConVar and ConCommand

CONVAR(test_cvar, "test var", "0", FCVAR_NONE);

CON_COMMAND(test_cmd, "test command", FCVAR_NONE) {
    Msg("Hello\n");
    SetValue(test_cvar, "1");
}

bool LoadExampleModule(void) {
    InitConVar(test_cvar);
    InitCommand(test_cmd);
    return true;
}

void UnloadExampleModule(void) {}
