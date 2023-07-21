#include "interfaces.h"

CreateInterfaceFn engine_factory = NULL;
CreateInterfaceFn server_factory = NULL;

ICvar* icvar = NULL;
IVEngineServer* engine_server = NULL;
IGameEventManager2* game_event_mgr = NULL;
IMatSystemSurface* mat_system_surface = NULL;
IEngineVGui* engine_vgui = NULL;
ISchemeManager* scheme_mgr = NULL;
IPanel* ipanel = NULL;
IScheme* ischeme = NULL;
