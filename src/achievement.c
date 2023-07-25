#include "achievement.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bonusmap.h"
#include "convar.h"
#include "dbg.h"
#include "hook.h"
#include "hud.h"
#include "interfaces.h"
#include "toast.h"

IAchievementMgr* achievement_mgr = NULL;

DECL_IFUNC(PRIVATE, IAchievementMgr*, engine_server, GetAchievementMgr, 100);

DECL_VFUNC(PRIVATE, void, ListenForEvents, 3);

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
    ConsoleColorPrintf(&green, "Achievement Unlocked!\n");

    char cmd[256] = "";
    snprintf(cmd, sizeof(cmd), "echo #%s#", achievement_name);
    ClientCmd(cmd);

    ToastAddAchieved(achievement_name);
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

        ToastAddProgress(achievement_name, cur_val, max_val);
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

#define ACH_HAS_COMPONENTS 0x0020
static void AchievementReset(int index) {
    IAchievement* iach =
        (*achievement_mgr)->GetAchievementByIndex(achievement_mgr, index);
    CBaseAchievement* base = GetBaseAchievement(iach);

    const char* name = (*iach)->GetName(iach);

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
            StopListeningForAllEvents(&ach->base1);                       \
            ListenForEvents(ach);                                         \
            Msg("%s reset!\n", name);                                     \
            return;                                                       \
        }                                                                 \
    } while (0)

    int achievement_id = (*iach)->GetAchievementID(iach);
    int goal = (*iach)->GetGoal(iach);
    int count = (*iach)->GetCount(iach);
    ACHIEVEMENT_TRY_RESET(CBaseAchievementV1);
    ACHIEVEMENT_TRY_RESET(CBaseAchievementV2);
    ACHIEVEMENT_TRY_RESET(CBaseAchievement);

    Msg("Failed to reset achievement.\n");
}

static void AchievmentResetAll(void) {
    int count = (*achievement_mgr)->GetAchievementCount(achievement_mgr);
    for (int i = 0; i < count; i++) {
        AchievementReset(i);
    }
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

    AchievementReset(index);
}

CON_COMMAND(sar_achievement_reset_all, "Clears all achievements", FCVAR_NONE) {
    AchievmentResetAll();
}

static void AchievementUnlock(int index) {
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

    AchievementUnlock(atoi(args->argv[1]));
}

CON_COMMAND(sar_achievement_unlock_all, "Unlocks all achievements",
            FCVAR_NONE) {
    int count = (*achievement_mgr)->GetAchievementCount(achievement_mgr);
    for (int i = 0; i < count; i++) {
        AchievementUnlock(i);
    }
}

CON_COMMAND(sar_full_game_reset, "Resets all achievements and bonus map",
            FCVAR_NONE) {
    AchievmentResetAll();
    BonusMapReset();
    ConVar* sv_unlockedchapters = FindVar("sv_unlockedchapters");
    if (sv_unlockedchapters) {
        const char* default_val = sv_unlockedchapters->parent->default_val;
        SetValue(sv_unlockedchapters, default_val);
    }

    ConVar* sv_cheats = FindVar("sv_cheats");
    if (sv_cheats) {
        SetIntValue(sv_cheats, 0);
    }
}

CONVAR(sar_hud_infinite_fall,
       "Draws Portal infinite fall achievement HUD\n"
       "  1 - ft\n"
       "  2 - HU",
       0, FCVAR_NONE);

#define ACHIEVEMENT_PORTAL_INFINITEFALL 119
static void OnPaint(void) {
    static HFont hud_font = 0;

    int mode = sar_hud_infinite_fall->val;
    if (!mode)
        return;

    CBaseAchievement* base =
        (*achievement_mgr)
            ->GetAchievementByID(achievement_mgr,
                                 ACHIEVEMENT_PORTAL_INFINITEFALL);
    if (!base)
        return;

    IAchievement* iach = &base->base2;

    float dist = -1;
    if ((*iach)->IsAchieved(iach)) {
        dist = 30000 * 12;
    } else {
#define ACHIEVEMENT_TRY_GET_DIST(T)                                           \
    do {                                                                      \
        if (dist == -1) {                                                     \
            T* ach = (T*)base;                                                \
            if (ach->achievement_id == achievement_id && ach->goal == goal && \
                ach->count == count) {                                        \
                struct {                                                      \
                    T base;                                                   \
                    bool is_flinging;                                         \
                    float accumulated_dist;                                   \
                    float z_portal_pos;                                       \
                }* fall_ach = (void*)ach;                                     \
                dist = fall_ach->accumulated_dist;                            \
            }                                                                 \
        }                                                                     \
    } while (0)

        int achievement_id = (*iach)->GetAchievementID(iach);
        int goal = (*iach)->GetGoal(iach);
        int count = (*iach)->GetCount(iach);
        ACHIEVEMENT_TRY_GET_DIST(CBaseAchievementV1);
        ACHIEVEMENT_TRY_GET_DIST(CBaseAchievementV2);
        ACHIEVEMENT_TRY_GET_DIST(CBaseAchievement);
    }

    if (dist == -1)
        return;

    if (!hud_font)
        hud_font = GetFont("Trebuchet24", false);

    if (!hud_font)
        return;

    DrawSetTextFont(hud_font);
    DrawSetTextColor((Color){255, 255, 255, 255});
    wchar_t buf[32];
    int len = 0;
    if (mode == 1) {
        len = swprintf_s(buf, sizeof(buf), L"Infinite fall: %d ft",
                         (int)(dist / 12));
    } else {
        len = swprintf_s(buf, sizeof(buf), L"Infinite fall: %.2f HU", dist);
    }

    int tw, th;
    GetTextSize(hud_font, buf, &tw, &th);
    DrawSetTextPos((screen_width - tw) / 2, screen_height * 0.1);
    DrawPrintText(buf, len, FONT_DRAW_DEFAULT);
}

static bool should_unhook;
static bool Load(void) {
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
    InitCommand(sar_full_game_reset);
    InitConVar(sar_hud_infinite_fall);

    return true;
}

static void Unload(void) {
    // Unhook
    if (should_unhook) {
        UnhookVirtual(HOOK_IFUNC(achievement_mgr, AwardAchievement),
                      orig_AwardAchievement);
    }
    StopListeningForAllEvents(&listener);
}

Module achievement_module = {
    .Load = &Load,
    .Unload = &Unload,
    .callbakcs[MODULE_ON_PAINT] = &OnPaint,
};
