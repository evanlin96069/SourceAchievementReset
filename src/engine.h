#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

#define INTERFACEVERSION_VENGINESERVER "VEngineServer021"
#define VENGINE_CLIENT_INTERFACE_VERSION_13 "VEngineClient013"
#define VENGINE_CLIENT_INTERFACE_VERSION_14 "VEngineClient014"

typedef void* IVEngineServer;
typedef void* IVEngineClient;

void ClientCmd(const char* command);

bool LoadEngineModule(void);
void UnloadEngineModule(void);

#endif
