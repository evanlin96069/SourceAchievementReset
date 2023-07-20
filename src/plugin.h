#ifndef PLUGIN_H
#define PLUGIN_H

#include "gameevents.h"

#define PLUGIN_NAME "SourceAchievementReset"
#define PLUGIN_VERSION "v0.0.1"

#define INTERFACEVERSION_ISERVERPLUGINCALLBACKS "ISERVERPLUGINCALLBACKS002"
#define INTERFACEVERSION_VENGINESERVER "VEngineServer021"

typedef void *(*CreateInterfaceFn)(const char *name, int *ret);

extern CreateInterfaceFn engine_factory;
extern CreateInterfaceFn server_factory;

extern void **engine_server;

#endif
