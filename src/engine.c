#include "engine.h"

#include "dbg.h"
#include "interfaces.h"
#include "vcall.h"

DECL_IFUNC(PUBLIC, void, engine_client, ClientCmd, 7, const char*);

bool LoadEngineModule(void) {
    engine_server = engine_factory(INTERFACEVERSION_VENGINESERVER, NULL);
    if (!engine_server) {
        Warning("Failed to get IVEngineServer interface.\n");
        return false;
    }

    engine_client = engine_factory(VENGINE_CLIENT_INTERFACE_VERSION_13, NULL);
    if (!engine_client) {
        engine_client =
            engine_factory(VENGINE_CLIENT_INTERFACE_VERSION_14, NULL);
    }
    if (!engine_client) {
        Warning("Failed to get IVEngineClient interface.\n");
        return false;
    }

    return true;
}

void UnloadEngineModule(void) {}
