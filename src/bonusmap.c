#include "bonusmap.h"

#include <string.h>

#include "dbg.h"
#include "interfaces.h"

static IGameUI* igameui = NULL;
BonusMapsDatabase_func BonusMapsDatabase = NULL;

CON_COMMAND(sar_bonusmap_reset, "Reset the bonus map database", FCVAR_NONE) {
    CBonusMapsDatabase* db = BonusMapsDatabase();

    KeyValues* files = KvFindKey(db->bonus_map_saved_data, "bonusfiles");
    if (!files)
        return;

    KvFree(files->sub);
    files->sub = NULL;
    KvFree(files->next);
    files->next = NULL;
}

static BonusMapsDatabase_func FindBonusMapsDatabase() {
    const int vtidx_BonusMapDatabaseSave = 27;
    uint8_t* base = (*(uint8_t***)(igameui))[vtidx_BonusMapDatabaseSave];
    uint8_t inst = *base;

    // CALL
    if (inst != 0xE8) {
        return NULL;
    }

    base += 1;
    int32_t offset = *(int32_t*)(base);
    base += 4;

    return (BonusMapsDatabase_func)(base + offset);
}

bool LoadBonusMapModule(void) {
    CreateInterfaceFn gameUIFactory = Sys_GetFactory("gameui.dll");
    igameui = (IGameUI*)gameUIFactory(GAMEUI_INTERFACE_VERSION, NULL);
    if (!igameui) {
        Msg("Failed to get IGameUI interface.\n");
        return false;
    }

    BonusMapsDatabase = FindBonusMapsDatabase();
    if (!BonusMapsDatabase) {
        Msg("Failed to find BonusMapsDatabase.\n");
        return false;
    }

    InitCommand(sar_bonusmap_reset);

    return true;
}

void UnloadBonusMapModule(void) {}
