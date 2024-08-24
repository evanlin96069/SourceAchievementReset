#include "engine.h"

#include "dbg.h"
#include "interfaces.h"
#include "vcall.h"

DECL_IFUNC(PUBLIC, void, engine_client, ClientCmd, 7, const char*);
DECL_IFUNC(PUBLIC, bool, engine_client, IsInGame, 26);

static bool Load(void) {
    engine_server = engine_factory(INTERFACEVERSION_VENGINESERVER, NULL);
    if (!engine_server) {
        Warning("Failed to get IVEngineServer interface.\n");
        return false;
    }

    // Fun fact: Steampipe supports both 13 and 14
    engine_client = engine_factory(VENGINE_CLIENT_INTERFACE_VERSION_14, NULL);
    if (engine_client) {
        engine_client_version = 14;
    } else {
        engine_client =
            engine_factory(VENGINE_CLIENT_INTERFACE_VERSION_13, NULL);
        if (engine_client) {
            engine_client_version = 13;
        } else {
            Warning("Failed to get IVEngineClient interface.\n");
            return false;
        }
    }

    return true;
}

static void Unload(void) {}

Module engine_module = {
    .Load = &Load,
    .Unload = &Unload,
};
