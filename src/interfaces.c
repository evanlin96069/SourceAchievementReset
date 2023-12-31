#include "interfaces.h"

#include <windows.h>

#define CREATEINTERFACE_PROCNAME "CreateInterface"

CreateInterfaceFn engine_factory = NULL;
CreateInterfaceFn server_factory = NULL;

ICvar* icvar = NULL;
IVEngineServer* engine_server = NULL;
IVEngineClient* engine_client = NULL;
int engine_client_version = 0;
IGameEventManager2* game_event_mgr = NULL;
IMatSystemSurface* mat_system_surface = NULL;
ISchemeManager* scheme_mgr = NULL;
IPanel* ipanel = NULL;
IScheme* ischeme = NULL;
ILocalize* ilocalize = NULL;
struct CServerPlugin* plugin_handler = NULL;

static inline void* Sys_GetProcAddress(const char* pModuleName,
                                       const char* pName) {
    HMODULE hModule = (HMODULE)GetModuleHandle(pModuleName);
    if (!hModule)
        return NULL;

    void* factory = (void*)GetProcAddress(hModule, pName);

    CloseHandle(hModule);
    return factory;
}

CreateInterfaceFn Sys_GetFactory(const char* pModuleName) {
    return (CreateInterfaceFn)(Sys_GetProcAddress(pModuleName,
                                                  CREATEINTERFACE_PROCNAME));
}
