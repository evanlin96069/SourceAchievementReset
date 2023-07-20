#ifndef PLUGIN_H
#define PLUGIN_H

#define PLUGIN_NAME "SourceAchievementReset"
#define PLUGIN_VERSION "v0.0.1"

#define INTERFACEVERSION_ISERVERPLUGINCALLBACKS "ISERVERPLUGINCALLBACKS002"

typedef void *(*CreateInterfaceFn)(const char *name, int *ret);

extern CreateInterfaceFn engine_factory;
extern CreateInterfaceFn server_factory;

#endif
