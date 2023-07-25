#include "module.h"

#include "achievement.h"
#include "bonusmap.h"
#include "convar.h"
#include "engine.h"
#include "gameevents.h"
#include "hud.h"

// clang-format off

Module* modules[] = {
    &cvar_module,
    &engine_module,
    &gameevents_module,
    &hud_module,
    &achievement_module,
    &bonusmap_module,
};

// clang-format on

const int module_count = sizeof(modules) / sizeof(Module*);

bool LoadModules(void) {
    for (int i = 0; i < module_count; i++) {
        modules[i]->is_loaded = modules[i]->Load();
        if (!modules[i]->is_loaded) {
            return false;
        }
    }

    return true;
}

void UnloadModules(void) {
    for (int i = module_count - 1; i >= 0; i--) {
        if (modules[i]->is_loaded) {
            modules[i]->Unload();
        }
    }
}

void SingalModules(ModuleSignalType type) {
    for (int i = 0; i < module_count; i++) {
        if (modules[i]->is_loaded && modules[i]->callbakcs[type]) {
            modules[i]->callbakcs[type]();
        }
    }
}
