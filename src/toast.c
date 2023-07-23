#include "toast.h"

#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "convar.h"
#include "hud.h"

#define TOAST_MAX 16

#define TOAST_WIDTH 240
#define TOAST_HIEGHT 120
#define TOAST_ALPHA 225.0f

#define TOAST_PADDING 4
#define TOAST_TEXT_PADDING 16

#define TOAST_SPEED 1200.0f
#define TOAST_DURATION 10000

typedef struct Toast {
    wchar_t title[TOAST_STRING_MAX];
    wchar_t desc[TOAST_STRING_MAX];
    clock_t duration;
    int pos;
    int alpha;
} Toast;

typedef struct ToastQueue {
    Toast data[TOAST_MAX];
    int front;
    int back;
    int size;
} ToastQueue;

ToastQueue toast_queue = {0};

CONVAR(sar_toast, "Draw achievement toast HUD", 1, FCVAR_DONTRECORD);

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

void ToastAdd(const wchar_t title[TOAST_STRING_MAX],
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
    new_toast->duration = TOAST_DURATION;
    new_toast->pos = 0;
    new_toast->alpha = TOAST_ALPHA;
    memcpy(new_toast->title, title, TOAST_STRING_MAX);
    memcpy(new_toast->desc, desc, TOAST_STRING_MAX);
}

static inline void ToastDraw(Toast toast) {
    static HFont title_font = 0;
    static HFont desc_font = 0;

    if (!title_font)
        title_font = GetFont("AchievementTitleFont", false);

    if (!desc_font)
        desc_font = GetFont("AchievementDescriptionFont", false);

    if (!title_font || !desc_font)
        return;

    int top_x = screen_width - TOAST_WIDTH;
    int top_y = screen_height - toast.pos;

    DrawSetColor((Color){0, 0, 0, toast.alpha});
    DrawFilledRect(top_x, top_y, top_x + TOAST_WIDTH, top_y + TOAST_HIEGHT);

    top_x += TOAST_TEXT_PADDING;
    top_y += TOAST_HIEGHT / 4;

    DrawSetTextColor((Color){255, 255, 255, toast.alpha});
    DrawSetTextFont(title_font);
    DrawSetTextPos(top_x, top_y);
    DrawPrintText(toast.title, wcslen(toast.title), FONT_DRAW_DEFAULT);

    int font_tall = GetFontTall(desc_font);
    DrawSetTextFont(desc_font);
    DrawSetTextPos(top_x, top_y + font_tall * 2);
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
            int target_pos =
                (TOAST_HIEGHT + TOAST_PADDING) * (count + 1) - TOAST_PADDING;
            if (toast_queue.data[i].pos < target_pos) {
                toast_queue.data[i].pos +=
                    TOAST_SPEED * frame_time / CLOCKS_PER_SEC;
                if (toast_queue.data[i].pos > target_pos) {
                    toast_queue.data[i].pos = target_pos;
                }
            }

            if (toast_queue.data[i].duration < CLOCKS_PER_SEC) {
                toast_queue.data[i].alpha = (TOAST_ALPHA / CLOCKS_PER_SEC) *
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
