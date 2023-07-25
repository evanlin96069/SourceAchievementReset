#include "gameevents.h"

#include <stddef.h>

#include "dbg.h"
#include "interfaces.h"

bool LoadGameEventsModule(void) {
    game_event_mgr = engine_factory(INTERFACEVERSION_GAMEEVENTSMANAGER2, NULL);
    if (!game_event_mgr) {
        Warning("Failed to get IGameEventManager2 interface.\n");
        return false;
    }

    return true;
}

void UnloadGameEventsModule(void) {}