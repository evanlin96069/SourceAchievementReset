#include "hud.h"

#include "achievement.h"
#include "dbg.h"
#include "hook.h"
#include "interfaces.h"
#include "mem.h"
#include "x86.h"

static void *tools_panel = NULL;
DECL_IFUNC(PRIVATE, void, tools_panel, SetPaintEnabled, 67, bool);

DECL_IFUNC(PUBLIC, void, mat_system_surface, DrawSetColor, 10, Color);
DECL_IFUNC(PUBLIC, void, mat_system_surface, DrawFilledRect, 12, int, int, int,
           int);
DECL_IFUNC(PUBLIC, void, mat_system_surface, DrawOutlinedRect, 14, int, int,
           int, int);
DECL_IFUNC(PUBLIC, void, mat_system_surface, DrawLine, 15, int, int, int, int);
DECL_IFUNC(PUBLIC, void, mat_system_surface, DrawPolyLine, 16, int *, int *,
           int);
DECL_IFUNC(PUBLIC, void, mat_system_surface, DrawSetTextFont, 17, HFont);
DECL_IFUNC(PUBLIC, void, mat_system_surface, DrawSetTextColor, 18, Color);
DECL_IFUNC(PUBLIC, void, mat_system_surface, DrawSetTextPos, 20, int, int);
DECL_IFUNC(PUBLIC, void, mat_system_surface, DrawPrintText, 22, const wchar_t *,
           int, FontDrawType_t);
DECL_IFUNC(PUBLIC, void, mat_system_surface, GetScreenSize, 37, int *, int *);
DECL_IFUNC(PUBLIC, HFont, mat_system_surface, CreateFont, 64);
DECL_IFUNC(PUBLIC, int, mat_system_surface, GetFontTall, 67, HFont);
DECL_IFUNC(PUBLIC, int, mat_system_surface, GetCharacterWidth, 71, HFont, int);

static void OnPaint(void) {}

static const int vtidx_Paint = 123;
typedef void (*virtual Paint_func)(void *);
static Paint_func orig_Paint;
static void virtual Hook_Paint(void *this) {
    if (this == tools_panel) {
        OnPaint();
    }
    orig_Paint(this);
}

// Not sure what's going on here, but I trust sst:)
static inline bool FindEngineToolsPanel(void *vgui_getpanel) {
    for (uint8_t *p = (uint8_t *)vgui_getpanel;
         p - (uint8_t *)vgui_getpanel < 16;) {
        // first CALL instruction in GetPanel calls GetRootPanel, which gives a
        // pointer to the specified panel
        if (p[0] == X86_CALL) {
            void *(*__thiscall GetRootPanel)(void *this, int panel_type);
            int offset = mem_load32(p + 1);
            p += x86_len(p);
            GetRootPanel = (void *(*__thiscall)(void *, int))(p + offset);
            tools_panel = GetRootPanel(engine_vgui, PANEL_TOOLS);
            return true;
        }
        NEXT_INSN(p);
    }
    return false;
}

static bool should_unhook;
bool LoadHudModule(void) {
    should_unhook = false;
    mat_system_surface =
        engine_factory(MAT_SYSTEM_SURFACE_INTERFACE_VERSION, NULL);
    if (!mat_system_surface) {
        Warning("Failed to get IMatSystemSurface interface.\n");
        return false;
    }

    engine_vgui = engine_factory(VENGINE_VGUI_VERSION, NULL);
    if (!engine_vgui) {
        Warning("Failed to get IEngineVGui interface.\n");
        return false;
    }

    scheme_mgr = engine_factory(VGUI_SCHEME_INTERFACE_VERSION, NULL);
    if (!scheme_mgr) {
        Warning("Failed to get ISchemeManager interface.\n");
        return false;
    }

    if (!FindEngineToolsPanel((*engine_vgui)->GetPanel)) {
        Warning("Failed to find engine tools panel.\n");
        return false;
    }

    orig_Paint =
        HookVirtual(&(*(void ***)tools_panel)[vtidx_Paint], &Hook_Paint);
    if (!orig_Paint) {
        Warning("Failed to hook Paint.\n");
        return false;
    }

    should_unhook = true;

    SetPaintEnabled(true);

    return true;
}

void UnloadHudModule(void) {
    if (should_unhook) {
        UnhookVirtual(&(*(void ***)tools_panel)[vtidx_Paint], orig_Paint);
        SetPaintEnabled(false);
    }
}
