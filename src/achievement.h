#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <stdint.h>

#include "gameevents.h"
#include "vcall.h"

#define ACH_LISTEN_KILL_EVENTS 0x0001
#define ACH_LISTEN_MAP_EVENTS 0x0002
#define ACH_LISTEN_COMPONENT_EVENTS 0x0004
#define ACH_HAS_COMPONENTS 0x0020
#define ACH_SAVE_WITH_GAME 0x0040
#define ACH_SAVE_GLOBAL 0x0080
#define ACH_FILTER_ATTACKER_IS_PLAYER 0x0100
#define ACH_FILTER_VICTIM_IS_PLAYER_ENEMY 0x0200
#define ACH_FILTER_FULL_ROUND_ONLY 0x0400

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
    bool (*virtual ShouldShowOnHUD)(void* this);
    void (*virtual SetShowOnHUD)(void* this, bool bShow);
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
    bool (*virtual HasAchieved)(void* this, const char* pchName);
    bool (*virtual WereCheatsEverOn)(void* this);
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
};

bool LoadAchievementModule(void);
void UnloadAchievementModule(void);

#endif
