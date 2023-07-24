#include "toast.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "convar.h"
#include "hud.h"

#define TOAST_MAX 16

#define TOAST_WIDTH 285
#define TOAST_HIEGHT 70

#define TOAST_FONT_NAME "AchievementItemDescription"
#define TOAST_PADDING 4
#define TOAST_IMAGE_PADDING 14
#define TOAST_TEXT_PADDING 2
#define TOAST_PROGRESS_HEIGHT 8

#define TOAST_SPEED 1200
#define TOAST_DURATION 10000
#define TOAST_FADEOUT_TIME 500

typedef struct Toast {
    wchar_t title[TOAST_STRING_MAX];
    wchar_t desc[TOAST_STRING_MAX];
    int icon_id;
    bool is_progress;
    int count;
    int goal;

    int x;
    int y;
    clock_t duration;
} Toast;

typedef struct ToastQueue {
    Toast data[TOAST_MAX];
    int front;
    int back;
    int size;
} ToastQueue;

ToastQueue toast_queue = {0};

CONVAR(sar_toast, "Draws achievement toast HUD", 1, FCVAR_DONTRECORD);

static inline void ToastQueueInit(void) {
    toast_queue.front = -1;
    toast_queue.back = -1;
    toast_queue.size = 0;
}

static inline bool ToastQueueIsEmpty(void) { return toast_queue.size == 0; }

static inline bool ToastQueueIsFull(void) {
    return toast_queue.size == TOAST_MAX;
}

static inline void ToastQueuePop(void) {
    if (ToastQueueIsEmpty())
        return;

    if (toast_queue.front == toast_queue.back) {
        toast_queue.front = -1;
        toast_queue.back = -1;
    } else {
        toast_queue.front = (toast_queue.front + 1) % TOAST_MAX;
    }

    toast_queue.size--;
}

// Returns a pointer to the next position
static inline Toast* ToastQueuePush(void) {
    if (ToastQueueIsFull()) {
        ToastQueuePop();
    }

    if (ToastQueueIsEmpty()) {
        toast_queue.front = 0;
    }

    toast_queue.back = (toast_queue.back + 1) % TOAST_MAX;
    toast_queue.size++;

    return &toast_queue.data[toast_queue.back];
}

static inline const wchar_t* GetAchievementLocalizedName(const char* name) {
    char name_key[256] = "";
    snprintf(name_key, sizeof(name_key), "#%s_NAME", name);

    return ILocalizeFind(name_key);
}

static inline const wchar_t* GetAchievementLocalizedDesc(const char* name) {
    char name_key[256] = "";
    snprintf(name_key, sizeof(name_key), "#%s_DESC", name);

    return ILocalizeFind(name_key);
}

static inline int GetAchievementIconId(const char* achievement_name) {
    char filename[260] = "";
    snprintf(filename, sizeof(filename), "VGUI/achievements/%s",
             achievement_name);
    int id = DrawGetTextureId(filename);
    if (id == -1) {
        id = CreateNewTextureID(false);
    }
    DrawSetTextureFile(id, filename, true, false);
    return id;
}

void ToastAddAchieved(const char* achievement_name) {
    Toast* new_toast = ToastQueuePush();

    new_toast->is_progress = false;
    new_toast->duration = TOAST_DURATION;
    new_toast->x = TOAST_WIDTH;
    new_toast->y = 0;
    new_toast->icon_id = GetAchievementIconId(achievement_name);

    const wchar_t* localized_name =
        GetAchievementLocalizedName(achievement_name);
    const wchar_t* localized_desc =
        GetAchievementLocalizedDesc(achievement_name);

    if (!localized_name || !localized_desc)
        return;

    memset(new_toast->title, 0, sizeof(new_toast->title));
    memset(new_toast->desc, 0, sizeof(new_toast->desc));

    int len = wcslen(localized_name);
    len = len < TOAST_STRING_MAX ? len : TOAST_STRING_MAX - 1;
    memcpy(new_toast->title, localized_name, len * sizeof(wchar_t));

    len = wcslen(localized_desc);
    len = len < TOAST_STRING_MAX ? len : TOAST_STRING_MAX - 1;
    memcpy(new_toast->desc, localized_desc, len * sizeof(wchar_t));
}

void ToastAddProgress(const char* achievement_name, int count, int goal) {
    Toast* new_toast = ToastQueuePush();

    new_toast->is_progress = true;
    new_toast->duration = TOAST_DURATION;
    new_toast->x = TOAST_WIDTH;
    new_toast->y = 0;
    new_toast->icon_id = GetAchievementIconId(achievement_name);
    new_toast->count = count;
    new_toast->goal = goal;

    const wchar_t* localized_name =
        GetAchievementLocalizedName(achievement_name);

    if (!localized_name)
        return;

    memset(new_toast->title, 0, sizeof(new_toast->title));
    memset(new_toast->desc, 0, sizeof(new_toast->desc));

    int len = wcslen(localized_name);
    len = len < TOAST_STRING_MAX ? len : TOAST_STRING_MAX - 1;
    memcpy(new_toast->title, localized_name, len * sizeof(wchar_t));

    swprintf(new_toast->desc, TOAST_STRING_MAX, L" #   (%d/%d)", count, goal);
}

static inline void ToastDraw(const Toast* toast) {
    static HFont toast_font = 0;
    static int font_tall = 0;

    if (!toast_font) {
        toast_font = GetFont(TOAST_FONT_NAME, false);
        if (!toast_font)
            return;
        font_tall = GetFontTall(toast_font);
    }

    int top_x = screen_width - toast->x;
    int top_y = screen_height - toast->y;

    DrawSetColor((Color){23, 26, 33, 255});
    DrawFilledRect(top_x, top_y, top_x + TOAST_WIDTH, top_y + TOAST_HIEGHT);

    top_x += TOAST_IMAGE_PADDING;
    top_y += TOAST_IMAGE_PADDING;
    int icon_size = TOAST_HIEGHT - TOAST_IMAGE_PADDING * 2;
    DrawSetTexture(toast->icon_id);
    DrawSetColor((Color){255, 255, 255, 255});
    DrawTexturedRect(top_x, top_y, top_x + icon_size, top_y + icon_size);
    top_x += icon_size + TOAST_IMAGE_PADDING;
    // Center the text
    if (toast->is_progress) {
        top_y += (icon_size - (font_tall * 2 + TOAST_TEXT_PADDING * 3 +
                               TOAST_PROGRESS_HEIGHT)) /
                 2;
    } else {
        top_y += (icon_size - (font_tall * 2 + TOAST_TEXT_PADDING)) / 2;
    }

    DrawSetTextFont(toast_font);
    DrawSetTextColor((Color){255, 255, 255, 255});
    DrawSetTextPos(top_x, top_y);
    DrawPrintText(toast->title, wcslen(toast->title), FONT_DRAW_DEFAULT);

    top_y += font_tall + TOAST_TEXT_PADDING;
    DrawSetTextColor((Color){115, 115, 115, 255});
    DrawSetTextPos(top_x, top_y);
    DrawPrintText(toast->desc, wcslen(toast->desc), FONT_DRAW_DEFAULT);

    top_y += font_tall + TOAST_TEXT_PADDING * 2;

    // Draw progress bar
    if (toast->is_progress) {
        int progress_width = TOAST_WIDTH - TOAST_IMAGE_PADDING * 3 - icon_size;
        DrawSetColor((Color){61, 68, 80, 255});
        DrawFilledRect(top_x, top_y, top_x + progress_width,
                       top_y + TOAST_PROGRESS_HEIGHT);
        DrawSetColor((Color){26, 159, 255, 255});
        progress_width *= ((float)toast->count / toast->goal);
        DrawFilledRect(top_x, top_y, top_x + progress_width,
                       top_y + TOAST_PROGRESS_HEIGHT);
    }
}

void ToastOnPaint(void) {
    static clock_t prev = 0;
    clock_t curr = clock();
    clock_t frame_time = curr - prev;
    prev = curr;

    if (!sar_toast->val) {
        // Clear queue
        ToastQueueInit();
        return;
    }

    if (ToastQueueIsEmpty())
        return;

    int i = toast_queue.back;
    int count = 0;
    do {
        toast_queue.data[i].duration -= frame_time;
        if (toast_queue.data[i].duration < 0) {
            ToastQueuePop();
        } else {
            int target_y =
                (TOAST_HIEGHT + TOAST_PADDING) * (count + 1) - TOAST_PADDING;
            if (toast_queue.data[i].y < target_y) {
                toast_queue.data[i].y +=
                    (float)TOAST_SPEED * ((float)frame_time / CLOCKS_PER_SEC);
                if (toast_queue.data[i].y > target_y) {
                    toast_queue.data[i].y = target_y;
                }
            }

            if (toast_queue.data[i].duration < TOAST_FADEOUT_TIME) {
                toast_queue.data[i].x =
                    ((float)TOAST_WIDTH / TOAST_FADEOUT_TIME) *
                    toast_queue.data[i].duration;
            }

            ToastDraw(&toast_queue.data[i]);
        }

        count++;
        i--;
        if (i < 0) {
            i = TOAST_MAX - 1;
        }
    } while (count < toast_queue.size);
}

void ToastInit(void) {
    ToastQueueInit();
    InitConVar(sar_toast);
}
