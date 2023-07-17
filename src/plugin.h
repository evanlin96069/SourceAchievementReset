#ifndef PLUGIN_H
#define PLUGIN_H

typedef void *(*CreateInterfaceFn)(const char *name, int *ret);

extern CreateInterfaceFn engine_factory;
extern CreateInterfaceFn server_factory;

#endif
