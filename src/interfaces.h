#ifndef INTERFACES_H
#define INTERFACES_H

#include "convar.h"
#include "gameevents.h"

#define INTERFACEVERSION_VENGINESERVER "VEngineServer021"

typedef void* IVEngineServer;

extern ICvar* icvar;
extern IVEngineServer* engine_server;
extern IGameEventManager2* game_event_mgr;

#endif
