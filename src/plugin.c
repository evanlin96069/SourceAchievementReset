#include "plugin.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "achievement.h"
#include "bonusmap.h"
#include "convar.h"
#include "dbg.h"
#include "interfaces.h"
#include "vcall.h"

static bool plugin_loaded = false;
static bool skip_unload = false;

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

    engine_server = engine_factory(INTERFACEVERSION_VENGINESERVER, NULL);
    if (!engine_server) {
        Warning("Failed to get IVEngineServer interface.\n");
        return false;
    }

    game_event_mgr = engine_factory(INTERFACEVERSION_GAMEEVENTSMANAGER2, NULL);
    if (!game_event_mgr) {
        Warning("Failed to get IGameEventManager2 interface.\n");
        return false;
    }

    if (!LoadCvarModule())
        return false;

    if (!LoadHudModule())
        return false;

    if (!LoadAchievementModule())
        return false;

    if (!LoadBonusMapModule())
        return false;

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

    UnloadCvarModule();
    UnloadHudModule();
    UnloadAchievementModule();
    UnloadBonusMapModule();

    plugin_loaded = false;
}

static void virtual Pause(void *this) {}

static void virtual UnPause(void *this) {}

static const char *virtual GetPluginDescription(void *this) {
    return PLUGIN_NAME " v" PLUGIN_VERSION;
}

static void virtual LevelInit(void *this, const char *map_name) {}

static void virtual ServerActivate(void *this, void *p, int i1, int i2) {}

static void virtual GameFrame(void *this, bool b) {}

static void virtual LevelShutdown(void *this) {}

static void virtual ClientActive(void *this, void *p) {}

static void virtual ClientDisconnect(void *this, void *p) {}

static void virtual ClientPutInServer(void *this, void *p1, void *p2) {}

static void virtual SetCommandClient(void *this, int i) {}

static void virtual ClientSettingsChanged(void *this, void *p) {}

static int virtual ClientConnect(void *this, void *p1, void *p2, void *p3,
                                 void *p4, void *p5, int i) {
    return 0;
}

static int virtual ClientCommand(void *this, void *p1, void *p2) { return 0; }

static int virtual NetworkIDValidated(void *this, void *p1, void *p2) {
    return 0;
}

static void virtual OnQueryCvarValueFinished(void *this, int i1, void *p1,
                                             int i2, void *p2, void *p3) {}

static const void *vtable[20] = {
    (void *)&Load,
    (void *)&Unload,
    (void *)&Pause,
    (void *)&UnPause,
    (void *)&GetPluginDescription,
    (void *)&LevelInit,
    (void *)&ServerActivate,
    (void *)&GameFrame,
    (void *)&LevelShutdown,
    (void *)&ClientActive,
    (void *)&ClientDisconnect,
    (void *)&ClientPutInServer,
    (void *)&SetCommandClient,
    (void *)&ClientSettingsChanged,
    (void *)&ClientConnect,
    (void *)&ClientCommand,
    (void *)&NetworkIDValidated,
    (void *)&OnQueryCvarValueFinished,
};

static const void *const *const plugin_obj = vtable;
__declspec(dllexport) const void *CreateInterface(const char *name, int *ret) {
    if (strcmp(name, INTERFACEVERSION_ISERVERPLUGINCALLBACKS) == 0) {
        if (ret)
            *ret = 0;
        return &plugin_obj;
    }
    if (ret)
        *ret = 1;
    return NULL;
}
