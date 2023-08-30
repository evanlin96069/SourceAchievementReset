#ifndef PLUGIN_H
#define PLUGIN_H

#include "interfaces.h"
#include "utils.h"
#include "vcall.h"

#define PLUGIN_NAME "SourceAchievementReset"
#define PLUGIN_VERSION "0.0.2"

#define INTERFACEVERSION_ISERVERPLUGINCALLBACKS "ISERVERPLUGINCALLBACKS002"
#define INTERFACEVERSION_ISERVERPLUGINHELPERS "ISERVERPLUGINHELPERS001"

typedef abstract_class IServerPluginCallbacks *IServerPluginCallbacks;
typedef abstract_class IServerPluginHelpers *IServerPluginHelpers;
typedef struct CPlugin CPlugin;
typedef struct CServerPlugin CServerPlugin;

typedef enum {
    PLUGIN_CONTINUE = 0,
    PLUGIN_OVERRIDE,
    PLUGIN_STOP,
} PLUGIN_RESULT;

abstract_class IServerPluginCallbacks {
    bool (*virtual Load)(void *this, CreateInterfaceFn interfaceFactory,
                         CreateInterfaceFn gameServerFactory);
    void (*virtual Unload)(void *this);
    void (*virtual Pause)(void *this);
    void (*virtual UnPause)(void *this);
    const char *(*virtual GetPluginDescription)(void *this);
    void (*virtual LevelInit)(void *this, char const *pMapName);
    void (*virtual ServerActivate)(void *this, void *pEdictList, int edictCount,
                                   int clientMax);
    void (*virtual GameFrame)(void *this, bool simulating);
    void (*virtual LevelShutdown)(void *this);
    void (*virtual ClientActive)(void *this, void *pEntity);
    void (*virtual ClientDisconnect)(void *this, void *pEntity);
    void (*virtual ClientPutInServer)(void *this, void *pEntity,
                                      char const *playername);
    void (*virtual SetCommandClient)(void *this, int index);
    void (*virtual ClientSettingsChanged)(void *this, void *pEdict);
    PLUGIN_RESULT(*virtual ClientConnect)
    (void *this, bool *bAllowConnect, void *pEntity, const char *pszName,
     const char *pszAddress, char *reject, int maxrejectlen);
    PLUGIN_RESULT(*virtual ClientCommand)
    (void *this, void *pEntity, const CCommand *args);
    PLUGIN_RESULT(*virtual NetworkIDValidated)
    (void *this, const char *pszUserName, const char *pszNetworkID);
    void (*virtual OnQueryCvarValueFinished)(
        void *this, int iCookie, void *pPlayerEntity, int eStatus,
        const char *pCvarName, const char *pCvarValue);
};

struct CPlugin {
    char name[128];
    bool disable;

    IServerPluginCallbacks *plugin;
    int plugin_interface_version;
    void *plugin_module;
};

struct CServerPlugin {
    void *vtable;
    CUtlVector(CPlugin *) plugins;
    void *plugin_helper_check;
};

extern const IServerPluginCallbacks sar;

#endif
