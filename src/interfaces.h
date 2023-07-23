#ifndef INTERFACES_H
#define INTERFACES_H

#include "bonusmap.h"
#include "convar.h"
#include "gameevents.h"
#include "hud.h"

#define INTERFACEVERSION_VENGINESERVER "VEngineServer021"

typedef void* (*CreateInterfaceFn)(const char* name, int* ret);

extern CreateInterfaceFn engine_factory;
extern CreateInterfaceFn server_factory;

typedef void* IVEngineServer;

extern ICvar* icvar;
extern IVEngineServer* engine_server;
extern IGameEventManager2* game_event_mgr;
extern IMatSystemSurface* mat_system_surface;
extern IEngineVGui* engine_vgui;
extern ISchemeManager* scheme_mgr;
extern IPanel* ipanel;
extern IScheme* ischeme;
extern ILocalize* ilocalize;

CreateInterfaceFn Sys_GetFactory(const char* pModuleName);

#endif
