#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include <stdbool.h>

#include "vcall.h"

#define INTERFACEVERSION_GAMEEVENTSMANAGER2 "GAMEEVENTSMANAGER002"

typedef struct IGameEvent *IGameEvent;
typedef struct IGameEventListener2 *IGameEventListener2;
typedef struct IGameEventManager2 *IGameEventManager2;
typedef struct CGameEventListener CGameEventListener;

extern IGameEventManager2 *game_event_mgr;

struct IGameEvent {
    void (*VCALLCONV dtor)(void *thisptr);
    const char *(*VCALLCONV GetName)(void *thisptr);
    bool (*VCALLCONV IsReliable)(void *thisptr);
    bool (*VCALLCONV IsLocal)(void *thisptr);
    bool (*VCALLCONV IsEmpty)(void *thisptr, const char *keyName);
    bool (*VCALLCONV GetBool)(void *thisptr, const char *keyName,
                              bool defaultValue);
    int (*VCALLCONV GetInt)(void *thisptr, const char *keyName,
                            int defaultValue);
    float (*VCALLCONV GetFloat)(void *thisptr, const char *keyName,
                                float defaultValue);
    const char *(*VCALLCONV GetString)(void *thisptr, const char *keyName,
                                       const char *defaultValue);
    void (*VCALLCONV SetBool)(void *thisptr, const char *keyName, bool value);
    void (*VCALLCONV SetInt)(void *thisptr, const char *keyName, int value);
    void (*VCALLCONV SetFloat)(void *thisptr, const char *keyName, float value);
    void (*VCALLCONV SetString)(void *thisptr, const char *keyName,
                                const char *value);
};

struct IGameEventListener2 {
    void (*VCALLCONV dtor)(void *thisptr);
    void (*VCALLCONV FireGameEvent)(void *thisptr, IGameEvent *event);
};

struct IGameEventManager2 {
    void (*VCALLCONV dtor)(void *thisptr);
    int (*VCALLCONV LoadEventsFromFile)(void *thisptr, const char *filename);
    void (*VCALLCONV Reset)(void *thisptr);
    bool (*VCALLCONV AddListener)(void *thisptr, IGameEventListener2 *listener,
                                  const char *name, bool bServerSide);
    bool (*VCALLCONV FindListener)(void *thisptr, IGameEventListener2 *listener,
                                   const char *name);
    void (*VCALLCONV RemoveListener)(void *thisptr,
                                     IGameEventListener2 *listener);
    IGameEvent *(*VCALLCONV CreateEvent)(void *thisptr, const char *name,
                                         bool bForce);
    bool (*VCALLCONV FireEvent)(void *thisptr, IGameEvent *event,
                                bool bDontBroadcast);
    bool (*VCALLCONV FireEventClientSide)(void *thisptr, IGameEvent *event);
    IGameEvent *(*VCALLCONV DuplicateEvent)(void *thisptr, IGameEvent *event);
    void (*VCALLCONV FreeEvent)(void *thisptr, IGameEvent *event);
    bool (*VCALLCONV SerializeEvent)(void *thisptr, IGameEvent *event,
                                     void *buf);
    IGameEvent *(*VCALLCONV UnserializeEvent)(void *thisptr, void *buf);
};

struct CGameEventListener {
    IGameEventListener2 base;
    bool registered_for_events;
};

static inline void ListenForGameEvent(CGameEventListener *thisptr,
                                      const char *name, bool server_side) {
    thisptr->registered_for_events = true;
    (*game_event_mgr)
        ->AddListener(game_event_mgr, &(thisptr->base), name, server_side);
}

static inline void StopListeningForAllEvents(CGameEventListener *thisptr) {
    if (thisptr->registered_for_events) {
        (*game_event_mgr)->RemoveListener(game_event_mgr, &(thisptr->base));
        thisptr->registered_for_events = false;
    }
}

#endif
