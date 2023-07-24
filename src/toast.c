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

#define TOAST_SPEED 1200
#define TOAST_DURATION 10000
#define TOAST_FADEOUT_TIME 500

typedef struct Toast {
    wchar_t title[TOAST_STRING_MAX];
    wchar_t desc[TOAST_STRING_MAX];
    int icon_id;
    clock_t duration;
    int x;
    int y;
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

static inline void ToastQueuePop() {
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

void ToastAdd(const char* achievement_name,
              const wchar_t title[TOAST_STRING_MAX],
              const wchar_t desc[TOAST_STRING_MAX]) {
    if (ToastQueueIsFull()) {
        ToastQueuePop();
    }

    if (ToastQueueIsEmpty()) {
        toast_queue.front = 0;
    }

    toast_queue.back = (toast_queue.back + 1) % TOAST_MAX;
    toast_queue.size++;

    Toast* new_toast = &toast_queue.data[toast_queue.back];

    char filename[260] = "";
    snprintf(filename, sizeof(filename), "VGUI/achievements/%s",
             achievement_name);
    int id = DrawGetTextureId(filename);
    if (id == -1) {
        id = CreateNewTextureID(false);
    }
    DrawSetTextureFile(id, filename, true, false);
    new_toast->icon_id = id;

    new_toast->duration = TOAST_DURATION;
    new_toast->x = TOAST_WIDTH;
    new_toast->y = 0;
    memcpy(new_toast->title, title, TOAST_STRING_MAX * sizeof(wchar_t));
    memcpy(new_toast->desc, desc, TOAST_STRING_MAX * sizeof(wchar_t));
}

static inline void ToastDraw(Toast toast) {
    static HFont toast_font = 0;
    static int font_tall = 0;

    if (!toast_font) {
        toast_font = GetFont(TOAST_FONT_NAME, false);
        if (!toast_font)
            return;
        font_tall = GetFontTall(toast_font);
    }

    int top_x = screen_width - toast.x;
    int top_y = screen_height - toast.y;

    DrawSetColor((Color){23, 26, 33, 255});
    DrawFilledRect(top_x, top_y, top_x + TOAST_WIDTH, top_y + TOAST_HIEGHT);

    top_x += TOAST_IMAGE_PADDING;
    top_y += TOAST_IMAGE_PADDING;
    int icon_size = TOAST_HIEGHT - TOAST_IMAGE_PADDING * 2;
    DrawSetTexture(toast.icon_id);
    DrawSetColor((Color){255, 255, 255, 255});
    DrawTexturedRect(top_x, top_y, top_x + icon_size, top_y + icon_size);
    top_x += icon_size + TOAST_IMAGE_PADDING;
    // Center the text
    top_y += (icon_size - (font_tall * 2 + TOAST_TEXT_PADDING)) / 2;

    DrawSetTextFont(toast_font);
    DrawSetTextColor((Color){255, 255, 255, 255});
    DrawSetTextPos(top_x, top_y);
    DrawPrintText(toast.title, wcslen(toast.title), FONT_DRAW_DEFAULT);

    DrawSetTextColor((Color){115, 115, 115, 255});
    DrawSetTextPos(top_x, top_y + font_tall + TOAST_TEXT_PADDING);
    DrawPrintText(toast.desc, wcslen(toast.desc), FONT_DRAW_DEFAULT);
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

            ToastDraw(toast_queue.data[i]);
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
