#ifndef HUD_H
#define HUD_H

#include <stdint.h>

#include "color.h"
#include "vcall.h"

#define MAT_SYSTEM_SURFACE_INTERFACE_VERSION "MatSystemSurface006"
#define VENGINE_VGUI_VERSION "VEngineVGui001"
#define VGUI_SCHEME_INTERFACE_VERSION "VGUI_Scheme010"

typedef void* IMatSystemSurface;
typedef void* ISchemeManager;
typedef abstract_class IEngineVGui* IEngineVGui;

typedef enum VGuiPanel_t {
    PANEL_ROOT = 0,
    PANEL_GAMEUIDLL,
    PANEL_CLIENTDLL,
    PANEL_TOOLS,
    PANEL_INGAMESCREENS,
    PANEL_GAMEDLL,
    PANEL_CLIENTDLL_TOOLS
} VGuiPanel_t;

typedef enum FontDrawType_t {
    FONT_DRAW_DEFAULT = 0,
    FONT_DRAW_NONADDITIVE,
    FONT_DRAW_ADDITIVE,
    FONT_DRAW_TYPE_COUNT = 2,
} FontDrawType_t;

abstract_class IEngineVGui {
    void (*virtual dtor)(void* this);
    uint32_t (*virtual GetPanel)(void* this, VGuiPanel_t type);
    bool (*virtual IsGameUIVisible)(void* this);
};

typedef uint32_t HFont;

// ISurface
void DrawSetColor(Color col);
void DrawFilledRect(int x0, int y0, int x1, int y1);
void DrawOutlinedRect(int x0, int y0, int x1, int y1);
void DrawLine(int x0, int y0, int x1, int y1);
void DrawPolyLine(int* px, int* py, int numPoints);
void DrawSetTextFont(HFont font);
void DrawSetTextColor(Color col);
void DrawSetTextPos(int x, int y);
void DrawPrintText(const wchar_t* text, int textLen, FontDrawType_t drawType);
void GetScreenSize(int* wide, int* tall);
HFont CreateFont();
int GetFontTall(HFont font);
int GetCharacterWidth(HFont font, int ch);

bool LoadHudModule(void);
void UnloadHudModule(void);

#endif
