#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <stdint.h>

#include "gameevents.h"
#include "module.h"
#include "vcall.h"

typedef abstract_class IAchievement* IAchievement;
typedef abstract_class IAchievementMgr* IAchievementMgr;

typedef struct CBaseAchievementV1 CBaseAchievementV1;
typedef struct CBaseAchievementV2 CBaseAchievementV2;
typedef struct CBaseAchievement CBaseAchievement;

extern IAchievementMgr* achievement_mgr;

abstract_class IAchievement {
    int (*virtual GetAchievementID)(void* this);
    const char* (*virtual GetName)(void* this);
    int (*virtual GetFlags)(void* this);
    int (*virtual GetGoal)(void* this);
    int (*virtual GetCount)(void* this);
    bool (*virtual IsAchieved)(void* this);
    int (*virtual GetPointValue)(void* this);
    bool (*virtual ShouldSaveWithGame)(void* this);
    bool (*virtual ShouldHideUntilAchieved)(void* this);
};

abstract_class IAchievementMgr {
    IAchievement* (*virtual GetAchievementByIndex)(void* this, int index);
    CBaseAchievement* (*virtual GetAchievementByID)(void* this, int id);
    int (*virtual GetAchievementCount)(void* this);
    void (*virtual InitializeAchievements)(void* this);
    void (*virtual AwardAchievement)(void* this, int iAchievementID);
    void (*virtual OnMapEvent)(void* this, const char* pchEventName);
    void (*virtual DownloadUserData)(void* this);
    void (*virtual EnsureGlobalStateLoaded)(void* this);
    void (*virtual SaveGlobalStateIfDirty)(void* this, bool bAsync);
};

// 3420
struct CBaseAchievementV1 {
    CGameEventListener base1;
    IAchievement base2;

    const char* name;
    int achievement_id;
    int flags;
    int goal;
    int progress_msg_increment;
    int progress_msg_min;
    int point_value;
    bool hide_until_achieved;
    bool store_progress_in_steam;
    const char* inflictor_class_name_filter;
    const char* inflictor_entity_name_filter;
    const char* victim_class_name_filter;
    const char* attacker_class_name_filter;
    const char* map_name_filter;
    const char* game_dir_filter;
    const char** component_names;
    int num_components;
    const char* component_prefix;
    int component_prefix_len;
    bool achieved;
    int count;
    int progress_shown;
    uint64_t component_bits;
    void* achievement_mgr;
};

// 5135
struct CBaseAchievementV2 {
    CGameEventListener base1;
    IAchievement base2;

    const char* name;
    int achievement_id;
    int flags;
    int goal;
    int progress_msg_increment;
    int progress_msg_min;
    int point_value;
    bool hide_until_achieved;
    bool store_progress_in_steam;
    const char* inflictor_class_name_filter;
    const char* inflictor_entity_name_filter;
    const char* victim_class_name_filter;
    const char* attacker_class_name_filter;
    const char* map_name_filter;
    const char* game_dir_filter;
    const char** component_names;
    int num_components;
    const char* component_prefix;
    int component_prefix_len;
    bool achieved;
    uint32_t unlock_time;  // New
    int count;
    int progress_shown;
    uint64_t component_bits;
    void* achievement_mgr;
};

// sdk2013
struct CBaseAchievement {
    CGameEventListener base1;
    IAchievement base2;

    const char* name;
    const char* stat;  // New
    int achievement_id;
    int flags;
    int goal;
    int progress_msg_increment;
    int progress_msg_min;
    int point_value;
    bool hide_until_achieved;
    bool store_progress_in_steam;
    const char* inflictor_class_name_filter;
    const char* inflictor_entity_name_filter;
    const char* victim_class_name_filter;
    const char* attacker_class_name_filter;
    const char* map_name_filter;
    const char* game_dir_filter;
    const char** component_names;
    int num_components;
    const char* component_prefix;
    int component_prefix_len;
    bool achieved;
    uint32_t unlock_time;
    int count;
    int progress_shown;
    uint64_t component_bits;
    void* achievement_mgr;
    bool show_on_HUD;  // New
};

extern Module achievement_module;

#endif
