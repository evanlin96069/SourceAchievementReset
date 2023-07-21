#include "achievement.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convar.h"
#include "dbg.h"
#include "hook.h"
#include "interfaces.h"
#include "toast.h"

IAchievementMgr* achievement_mgr = NULL;

DECL_IFUNC(PRIVATE, IAchievementMgr*, engine_server, GetAchievementMgr, 100);

typedef void (*virtual AwardAchievement_func)(void*, int);
static AwardAchievement_func orig_AwardAchievement = NULL;

static const Color white = {255, 255, 255, 255};
static const Color green = {0, 255, 0, 255};
static const Color red = {255, 0, 0, 255};
static const Color cyan = {0, 255, 255, 255};

static inline CBaseAchievement* GetBaseAchievement(IAchievement* iach) {
    return (CBaseAchievement*)((uint8_t*)iach - sizeof(CGameEventListener));
}

static void virtual Hooked_AwardAchievement(void* this, int id) {
    orig_AwardAchievement(this, id);
    CBaseAchievement* ach =
        (*achievement_mgr)->GetAchievementByID(achievement_mgr, id);
    if (!ach)
        return;
    IAchievement* iach = &ach->base2;
    if (!(*iach)->IsAchieved(iach))
        return;
    const char* achievement_name = (*iach)->GetName(iach);
    ConsoleColorPrintf(&green,
                       "Achievement Unlocked!\n"
                       "%s\n",
                       achievement_name);

    wchar_t title[TOAST_STRING_MAX] = {0};
    wchar_t desc1[TOAST_STRING_MAX] = L"Achievement Unlocked!";
    wchar_t desc2[TOAST_STRING_MAX] = L"";
    swprintf_s(title, TOAST_STRING_MAX, L"%s", achievement_name);
    ToastAdd(title, desc1, desc2);
}

static void virtual dtor(void* this) {}
static void virtual FireGameEvent(void* this, IGameEvent* event) {
    const char* event_name = (*event)->GetName(event);
    Msg("Event %s fired\n", event_name);
    if (strcmp(event_name, "achievement_event") == 0) {
        const char* achievement_name =
            (*event)->GetString(event, "achievement_name", NULL);
        int cur_val = (*event)->GetInt(event, "cur_val", -1);
        int max_val = (*event)->GetInt(event, "max_val", -1);
        ConsoleColorPrintf(&cyan,
                           "Achievement Progress!\n"
                           "%s (%d/%d)\n",
                           achievement_name, cur_val, max_val);

        wchar_t title[TOAST_STRING_MAX] = {0};
        wchar_t desc1[TOAST_STRING_MAX] = L"Achievement Progress!";
        wchar_t desc2[TOAST_STRING_MAX] = {0};
        swprintf_s(title, TOAST_STRING_MAX, L"%s", achievement_name);
        swprintf_s(desc2, TOAST_STRING_MAX, L"(%d/%d)", cur_val, max_val);
        ToastAdd(title, desc1, desc2);
    }
}

struct IGameEventListener2 listener_vtable = {
    &dtor,
    &FireGameEvent,
};

CGameEventListener listener = {
    .base = &listener_vtable,
    .registered_for_events = false,
};

CON_COMMAND(sar_achievement_status, "Shows status of all achievements",
            FCVAR_NONE) {
    int count = (*achievement_mgr)->GetAchievementCount(achievement_mgr);
    for (int i = 0; i < count; i++) {
        IAchievement* iach =
            (*achievement_mgr)->GetAchievementByIndex(achievement_mgr, i);

        ConsoleColorPrintf(&white, "%3d: ", i);
        if ((*iach)->IsAchieved(iach)) {
            ConsoleColorPrintf(&green, "[ DONE! ]");
        } else {
            ConsoleColorPrintf(&red, "[ %-2d/%2d ]", (*iach)->GetCount(iach),
                               (*iach)->GetGoal(iach));
        }
        ConsoleColorPrintf(&white, " %s\n", (*iach)->GetName(iach));
    }
}

static void ResetAchievement(int index) {
    IAchievement* iach =
        (*achievement_mgr)->GetAchievementByIndex(achievement_mgr, index);
    CBaseAchievement* base = GetBaseAchievement(iach);

    const char* name = (*iach)->GetName(iach);
    int achievement_id = (*iach)->GetAchievementID(iach);
    int goal = (*iach)->GetGoal(iach);
    int count = (*iach)->GetCount(iach);

#define ACHIEVEMENT_TRY_RESET(T)                                          \
    do {                                                                  \
        T* ach = (T*)base;                                                \
        if (ach->achievement_id == achievement_id && ach->goal == goal && \
            ach->count == count) {                                        \
            ach->achieved = false;                                        \
            ach->count = 0;                                               \
            if (ach->flags & ACH_HAS_COMPONENTS) {                        \
                ach->component_bits = 0;                                  \
            }                                                             \
            ach->progress_shown = 0;                                      \
            Msg("%s reset!\n", name);                                     \
            return;                                                       \
        }                                                                 \
    } while (0)

    ACHIEVEMENT_TRY_RESET(CBaseAchievementV1);
    ACHIEVEMENT_TRY_RESET(CBaseAchievementV2);
    ACHIEVEMENT_TRY_RESET(CBaseAchievement);

    Msg("Failed to reset achievement.\n");
}

CON_COMMAND(sar_achievement_reset, "Clears specified achievement", FCVAR_NONE) {
    if (args->argc != 2) {
        Msg("Usage: %s <index>\n", args->argv[0]);
        return;
    }

    int index = atoi(args->argv[1]);
    if (index < 0 ||
        index >= (*achievement_mgr)->GetAchievementCount(achievement_mgr)) {
        Msg("Invalid achievement index.\n");
        return;
    }

    ResetAchievement(index);
}

CON_COMMAND(sar_achievement_reset_all, "Clears all achievements", FCVAR_NONE) {
    int count = (*achievement_mgr)->GetAchievementCount(achievement_mgr);
    for (int i = 0; i < count; i++) {
        ResetAchievement(i);
    }
}

static void UnlockAchievement(int index) {
    IAchievement* iach =
        (*achievement_mgr)->GetAchievementByIndex(achievement_mgr, index);
    (*achievement_mgr)
        ->AwardAchievement(achievement_mgr, (*iach)->GetAchievementID(iach));
}

CON_COMMAND(sar_achievement_unlock, "Unlocks specified achievement",
            FCVAR_NONE) {
    if (args->argc != 2) {
        Msg("Usage: %s <index>\n", args->argv[0]);
        return;
    }

    int index = atoi(args->argv[1]);
    if (index < 0 ||
        index >= (*achievement_mgr)->GetAchievementCount(achievement_mgr)) {
        Msg("Invalid achievement index.\n");
        return;
    }

    UnlockAchievement(atoi(args->argv[1]));
}

CON_COMMAND(sar_achievement_unlock_all, "Unlocks all achievements",
            FCVAR_NONE) {
    int count = (*achievement_mgr)->GetAchievementCount(achievement_mgr);
    for (int i = 0; i < count; i++) {
        UnlockAchievement(i);
    }
}

static bool should_unhook;
bool LoadAchievementModule(void) {
    should_unhook = false;
    achievement_mgr = GetAchievementMgr();
    if (!achievement_mgr) {
        Warning("Failed to get IAchievementMgr interface.\n");
        return false;
    }

    // Hook
    orig_AwardAchievement =
        HookVirtual(HOOK_IFUNC(achievement_mgr, AwardAchievement),
                    &Hooked_AwardAchievement);
    if (!orig_AwardAchievement) {
        Warning("Failed to hook AwardAchievement.\n");
        return false;
    }

    should_unhook = true;

    ListenForGameEvent(&listener, "achievement_event", false);

    // Init commands
    InitCommand(sar_achievement_status);
    InitCommand(sar_achievement_reset);
    InitCommand(sar_achievement_reset_all);
    InitCommand(sar_achievement_unlock);
    InitCommand(sar_achievement_unlock_all);

    return true;
}

void UnloadAchievementModule(void) {
    // Unhook
    if (should_unhook) {
        UnhookVirtual(HOOK_IFUNC(achievement_mgr, AwardAchievement),
                      orig_AwardAchievement);
    }
    StopListeningForAllEvents(&listener);
}
