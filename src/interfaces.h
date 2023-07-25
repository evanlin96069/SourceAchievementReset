#ifndef INTERFACES_H
#define INTERFACES_H

#include "convar.h"
#include "engine.h"
#include "gameevents.h"
#include "hud.h"

typedef void* (*CreateInterfaceFn)(const char* name, int* ret);

extern CreateInterfaceFn engine_factory;
extern CreateInterfaceFn server_factory;

extern ICvar* icvar;
extern IVEngineServer* engine_server;
extern IVEngineClient* engine_client;
extern IGameEventManager2* game_event_mgr;
extern IMatSystemSurface* mat_system_surface;
extern ISchemeManager* scheme_mgr;
extern IPanel* ipanel;
extern IScheme* ischeme;
extern ILocalize* ilocalize;

CreateInterfaceFn Sys_GetFactory(const char* pModuleName);

#endif
