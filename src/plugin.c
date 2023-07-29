#include "plugin.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "dbg.h"
#include "module.h"

static bool plugin_loaded = false;
static bool skip_unload = false;

CON_COMMAND(sar_unload, "Unloads " PLUGIN_NAME, FCVAR_DONTRECORD) {
    for (int i = 0; i < plugin_handler->plugins.size; i++) {
        if (plugin_handler->plugins.element[i]->plugin == &sar) {
            char cmd[32];
            snprintf(cmd, sizeof cmd, "plugin_unload %d", i);
            ClientCmd(cmd);
            return;
        }
    }
    Msg("Could not determine plugin index.\n");
}

static bool virtual Load(void *this, CreateInterfaceFn interfaceFactory,
                         CreateInterfaceFn gameServerFactory) {
    if (plugin_loaded) {
        Warning("Plugin already loaded.\n");
        skip_unload = true;
        return false;
    }
    plugin_loaded = true;

    engine_factory = interfaceFactory;
    server_factory = gameServerFactory;

    if (!LoadModules())
        return false;

    plugin_handler =
        interfaceFactory(INTERFACEVERSION_ISERVERPLUGINHELPERS, NULL);
    if (plugin_handler) {
        InitCommand(sar_unload);
    }

    const Color green = {110, 247, 75, 255};
    ConsoleColorPrintf(&green, "Loaded %s, Version %s\n", PLUGIN_NAME,
                       PLUGIN_VERSION);

    return true;
}

static void virtual Unload(void *this) {
    if (skip_unload) {
        skip_unload = false;
        return;
    }
    UnloadModules();
    plugin_loaded = false;
}

static void virtual Pause(void *this) {}

static void virtual UnPause(void *this) {}

static const char *virtual GetPluginDescription(void *this) {
    return PLUGIN_NAME " v" PLUGIN_VERSION;
}

static void virtual LevelInit(void *this, char const *pMapName) {}

static void virtual ServerActivate(void *this, void *pEdictList, int edictCount,
                                   int clientMax) {}

static void virtual GameFrame(void *this, bool simulating) {}

static void virtual LevelShutdown(void *this) {}

static void virtual ClientActive(void *this, void *pEntity) {}

static void virtual ClientDisconnect(void *this, void *pEntity) {}

static void virtual ClientPutInServer(void *this, void *pEntity,
                                      char const *playername) {}

static void virtual SetCommandClient(void *this, int index) {}

static void virtual ClientSettingsChanged(void *this, void *pEdict) {}

static PLUGIN_RESULT virtual ClientConnect(void *this, bool *bAllowConnect,
                                           void *pEntity, const char *pszName,
                                           const char *pszAddress, char *reject,
                                           int maxrejectlen) {
    return PLUGIN_CONTINUE;
}

static PLUGIN_RESULT virtual ClientCommand(void *this, void *pEntity,
                                           const CCommand *args) {
    return PLUGIN_CONTINUE;
}

static PLUGIN_RESULT virtual NetworkIDValidated(void *this,
                                                const char *pszUserName,
                                                const char *pszNetworkID) {
    return PLUGIN_CONTINUE;
}

static void virtual OnQueryCvarValueFinished(void *this, int iCookie,
                                             void *pPlayerEntity, int eStatus,
                                             const char *pCvarName,
                                             const char *pCvarValue) {}

static abstract_class IServerPluginCallbacks vtable_iserverplugin = {
    &Load,
    &Unload,
    &Pause,
    &UnPause,
    &GetPluginDescription,
    &LevelInit,
    &ServerActivate,
    &GameFrame,
    &LevelShutdown,
    &ClientActive,
    &ClientDisconnect,
    &ClientPutInServer,
    &SetCommandClient,
    &ClientSettingsChanged,
    &ClientConnect,
    &ClientCommand,
    &NetworkIDValidated,
    &OnQueryCvarValueFinished,
};

const IServerPluginCallbacks sar = &vtable_iserverplugin;

__declspec(dllexport) const void *CreateInterface(const char *name, int *ret) {
    if (strcmp(name, INTERFACEVERSION_ISERVERPLUGINCALLBACKS) == 0) {
        if (ret)
            *ret = 0;
        return &sar;
    }
    if (ret)
        *ret = 1;
    return NULL;
}
