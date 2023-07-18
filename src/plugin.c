#include "plugin.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "convar.h"
#include "dbg.h"
#include "example.h"
#include "vcall.h"

#define INTERFACEVERSION_ISERVERPLUGINCALLBACKS "ISERVERPLUGINCALLBACKS002"

static bool plugin_loaded = false;
static bool skip_unload = false;

CreateInterfaceFn engine_factory = NULL;
CreateInterfaceFn server_factory = NULL;

static bool VCALLCONV Load(void *thisptr, CreateInterfaceFn interfaceFactory,
                           CreateInterfaceFn gameServerFactory) {
    if (plugin_loaded) {
        Warning("Plugin already loaded.\n");
        skip_unload = true;
        return false;
    }
    plugin_loaded = true;

    engine_factory = interfaceFactory;
    server_factory = gameServerFactory;

    if (!LoadCvarModule())
        return false;

    if (!LoadExampleModule())
        return false;

    return true;
}

static void VCALLCONV Unload(void *thisptr) {
    if (skip_unload) {
        skip_unload = false;
        return;
    }

    UnloadCvarModule();
    UnloadExampleModule();
    plugin_loaded = false;
}

static void VCALLCONV Pause(void *thisptr) {}

static void VCALLCONV UnPause(void *this) {}

static const char *VCALLCONV GetPluginDescription(void *thisptr) {
    return "example plugin";
}

static void VCALLCONV LevelInit(void *thisptr, const char *map_name) {}

static void VCALLCONV ServerActivate(void *thisptr, void *p, int i1, int i2) {}

static void VCALLCONV GameFrame(void *thisptr, bool b) {}

static void VCALLCONV LevelShutdown(void *thisptr) {}

static void VCALLCONV ClientActive(void *thisptr, void *p) {}

static void VCALLCONV ClientDisconnect(void *thisptr, void *p) {}

static void VCALLCONV ClientPutInServer(void *thisptr, void *p1, void *p2) {}

static void VCALLCONV SetCommandClient(void *thisptr, int i) {}

static void VCALLCONV ClientSettingsChanged(void *thisptr, void *p) {}

static int VCALLCONV ClientConnect(void *thisptr, void *p1, void *p2, void *p3,
                                   void *p4, void *p5, int i) {
    return 0;
}

static int VCALLCONV ClientCommand(void *thisptr, void *p1, void *p2) {
    return 0;
}

static int VCALLCONV NetworkIDValidated(void *thisptr, void *p1, void *p2) {
    return 0;
}

static void VCALLCONV OnQueryCvarValueFinished(void *thisptr, int i1, void *p1,
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
    return 0;
}
