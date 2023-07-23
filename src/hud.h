#ifndef HUD_H
#define HUD_H

#include <stdint.h>

#include "utils.h"
#include "vcall.h"

#define MAT_SYSTEM_SURFACE_INTERFACE_VERSION_6 "MatSystemSurface006"
#define MAT_SYSTEM_SURFACE_INTERFACE_VERSION_8 "MatSystemSurface008"
#define VENGINE_VGUI_VERSION "VEngineVGui001"
#define VGUI_SCHEME_INTERFACE_VERSION "VGUI_Scheme010"
#define VGUI_PANEL_INTERFACE_VERSION "VGUI_Panel009"
#define VGUI_LOCALIZE_INTERFACE_VERSION_4 "VGUI_Localize004"
#define VGUI_LOCALIZE_INTERFACE_VERSION_5 "VGUI_Localize005"

typedef void* IMatSystemSurface;
typedef void* ISchemeManager;
typedef void* IPanel;
typedef void* ILocalize;
typedef void* IScheme;
typedef abstract_class IEngineVGui* IEngineVGui;

typedef uint32_t VPANEL;

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

typedef unsigned long HScheme;
typedef unsigned long HFont;

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
int GetFontTall(HFont font);
int GetCharacterWidth(HFont font, int ch);

// ILocalize
const wchar_t* ILocalizeFind(char const* tokenName);

// IScheme
HFont GetFont(const char* fontName, bool proportional);

bool LoadHudModule(void);
void UnloadHudModule(void);

#endif
