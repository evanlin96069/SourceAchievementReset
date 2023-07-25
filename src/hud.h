#ifndef HUD_H
#define HUD_H

#include <stdint.h>

#include "module.h"
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

typedef uint32_t VPANEL;

typedef enum FontDrawType_t {
    FONT_DRAW_DEFAULT = 0,
    FONT_DRAW_NONADDITIVE,
    FONT_DRAW_ADDITIVE,
    FONT_DRAW_TYPE_COUNT = 2,
} FontDrawType_t;

typedef unsigned long HScheme;
typedef unsigned long HFont;

extern int screen_width;
extern int screen_height;

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

int DrawGetTextureId(const char* filename);
void DrawSetTextureFile(int id, const char* filename, int hardwareFilter,
                        bool forceReload);
void DrawSetTexture(int id);
void DrawGetTextureSize(int id, int* wide, int* tall);
void DrawTexturedRect(int x0, int y0, int x1, int y1);
int CreateNewTextureID(bool procedural);

void GetScreenSize(int* wide, int* tall);
int GetFontTall(HFont font);
void GetTextSize(HFont font, const wchar_t* text, int* wide, int* tall);

// ILocalize
const wchar_t* ILocalizeFind(char const* tokenName);

// IScheme
HFont GetFont(const char* fontName, bool proportional);

extern Module hud_module;

#endif
