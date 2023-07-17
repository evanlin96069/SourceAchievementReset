#include "plugin.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "convar.h"
#include "example.h"
#include "vcall.h"

#define INTERFACEVERSION_ISERVERPLUGINCALLBACKS "ISERVERPLUGINCALLBACKS003"

CreateInterfaceFn engine_factory = NULL;
CreateInterfaceFn server_factory = NULL;

static bool VCALLCONV Load(void *thisptr, CreateInterfaceFn interfaceFactory,
                           CreateInterfaceFn gameServerFactory) {
    engine_factory = interfaceFactory;
    server_factory = gameServerFactory;

    if (!LoadCvarModule())
        return false;

    if (!LoadExampleModule())
        return false;

    return true;
}

static void VCALLCONV Unload(void *thisptr) {
    UnloadCvarModule();
    UnloadExampleModule();
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

static void VCALLCONV OnEdictAllocated(void *thisptr, void *p) {}

static void VCALLCONV OnEdictFreed(void *thisptr, void *p) {}

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
    // ClientFullyConnect
    (void *)&ClientDisconnect,
    (void *)&ClientPutInServer,
    (void *)&SetCommandClient,
    (void *)&ClientSettingsChanged,
    (void *)&ClientConnect,
    (void *)&ClientCommand,
    (void *)&NetworkIDValidated,
    (void *)&OnQueryCvarValueFinished,
    (void *)&OnEdictAllocated,
    (void *)&OnEdictFreed,
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
