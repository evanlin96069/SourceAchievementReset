#ifndef BONUSMAP_H
#define BONUSMAP_H

#include <stdbool.h>
#include <stdint.h>

#include "keyvalues.h"
#include "utils.h"

#define GAMEUI_INTERFACE_VERSION "GameUI011"

typedef void *IGameUI;

typedef struct CUtlVector CUtlVector;
typedef struct ChallengeDescription_t ChallengeDescription_t;
typedef struct BonusMapDescription_t BonusMapDescription_t;
typedef struct BonusMapChallenge_t BonusMapChallenge_t;
typedef struct CBonusMapsDatabaseV1 CBonusMapsDatabaseV1;
typedef struct CBonusMapsDatabase CBonusMapsDatabase;

struct ChallengeDescription_t {
    char name[32];
    char comment[256];

    int type;

    int bronze;
    int silver;
    int gold;

    int best;
};

struct BonusMapDescription_t {
    bool is_folder;

    char short_name[64];
    char file_name[128];

    char map_file_name[128];
    char chapter_name[128];
    char image_name[128];

    char map_name[64];
    char comment[256];

    bool locked;
    bool complete;

    CUtlVector(ChallengeDescription_t) * challenges;
};

struct BonusMapChallenge_t {
    char szFileName[128];
    char szMapName[32];
    char szChallengeName[32];
    int iBest;
};

struct CBonusMapsDatabaseV1 {
    KeyValues *bonus_maps_manifest;

    CUtlVector(BonusMapDescription_t) bonus_maps;

    KeyValues *bonus_map_saved_data;
    bool saved_data_changed;

    int x360_bonuses_unlocked;  // Only used on 360
    bool has_loaded_save_data;

    int dir_depth;
    char current_path[260];
    float current_completion;
    int completable_levels;

    BonusMapChallenge_t current_challenge_names;
    ChallengeDescription_t current_challenge_objectives;
};

struct CBonusMapsDatabase {
    KeyValues *bonus_maps_manifest;
    void *unknown;  // New

    CUtlVector(BonusMapDescription_t) bonus_maps;

    KeyValues *bonus_map_saved_data;
    bool saved_data_changed;

    int x360_bonuses_unlocked;  // Only used on 360
    bool has_loaded_save_data;

    int dir_depth;
    char current_path[260];
    float current_completion;
    int completable_levels;

    BonusMapChallenge_t current_challenge_names;
    ChallengeDescription_t current_challenge_objectives;
};

typedef CBonusMapsDatabase *(*BonusMapsDatabase_func)(void);
extern BonusMapsDatabase_func BonusMapsDatabase;

void BonusMapReset(void);
bool LoadBonusMapModule(void);
void UnloadBonusMapModule(void);

#endif
