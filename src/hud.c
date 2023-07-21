#include "hud.h"

#include <string.h>

#include "achievement.h"
#include "dbg.h"
#include "hook.h"
#include "interfaces.h"

// IMatSystemSurface
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
static int vtidx_GetScreenSize = 0;
DECL_IFUNC_DYN(PUBLIC, void, mat_system_surface, GetScreenSize, int *, int *);
static int vtidx_GetFontTall = 0;
DECL_IFUNC_DYN(PUBLIC, int, mat_system_surface, GetFontTall, HFont);
static int vtidx_GetCharacterWidth = 0;
DECL_IFUNC_DYN(PUBLIC, int, mat_system_surface, GetCharacterWidth, HFont, int);

// IPanel
static int vtidx_IPanelGetName = 0;
DECL_IFUNC_DYN(PRIVATE, const char *, ipanel, IPanelGetName, VPANEL);

// ISchemeManager
DECL_IFUNC(PRIVATE, HScheme, scheme_mgr, GetDefaultScheme, 4);
DECL_IFUNC(PRIVATE, IScheme *, scheme_mgr, GetIScheme, 8, HScheme);

// IScheme
DECL_IFUNC(PUBLIC, HFont, ischeme, GetFont, 3, const char *, bool);

static void OnPaint(void) {}

static int vtidx_PaintTraverse = 0;
typedef void (*virtual PaintTraverse_func)(void *, VPANEL, bool, bool);
static PaintTraverse_func orig_PaintTraverse;
static void virtual Hook_PaintTraverse(void *this, VPANEL vguiPanel,
                                       bool forceRepaint, bool allowForce) {
    static VPANEL panel_id = 0;
    static bool found_panel_id = false;

    orig_PaintTraverse(this, vguiPanel, forceRepaint, allowForce);

    if (!found_panel_id) {
        if (strcmp(IPanelGetName(vguiPanel), "FocusOverlayPanel") == 0) {
            panel_id = vguiPanel;
            found_panel_id = true;
        }
    } else if (panel_id == vguiPanel) {
        // This is not perfect, but if we don't draw at the left side of the
        // screen it should be unnoticeable.
        OnPaint();
    }
}

static bool should_unhook;
bool LoadHudModule(void) {
    should_unhook = false;

    mat_system_surface =
        engine_factory(MAT_SYSTEM_SURFACE_INTERFACE_VERSION_6, NULL);
    if (mat_system_surface) {
        // 3420 & 5135
        vtidx_IPanelGetName = 35;
        vtidx_PaintTraverse = 40;

        vtidx_GetScreenSize = 37;
        vtidx_GetFontTall = 67;
        vtidx_GetCharacterWidth = 71;

    } else {
        mat_system_surface =
            engine_factory(MAT_SYSTEM_SURFACE_INTERFACE_VERSION_8, NULL);
        if (mat_system_surface) {
            // Steampipe
            vtidx_IPanelGetName = 36;
            vtidx_PaintTraverse = 41;

            vtidx_GetScreenSize = 38;
            vtidx_GetFontTall = 69;
            vtidx_GetCharacterWidth = 74;
        } else {
            Warning("Failed to get IMatSystemSurface interface.\n");
            return false;
        }
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

    ipanel = engine_factory(VGUI_PANEL_INTERFACE_VERSION, NULL);
    if (!ipanel) {
        Warning("Failed to get IPanel interface.\n");
        return false;
    }

    ischeme = GetIScheme(GetDefaultScheme());
    if (!ischeme) {
        Warning("Failed to get IScheme.\n");
        return false;
    }

    orig_PaintTraverse = HookVirtual(HOOK_VTABLE(ipanel, vtidx_PaintTraverse),
                                     &Hook_PaintTraverse);
    if (!orig_PaintTraverse) {
        Warning("Failed to hook Paint.\n");
        return false;
    }

    should_unhook = true;

    return true;
}

void UnloadHudModule(void) {
    if (should_unhook) {
        UnhookVirtual(HOOK_VTABLE(ipanel, vtidx_PaintTraverse),
                      orig_PaintTraverse);
    }
}
