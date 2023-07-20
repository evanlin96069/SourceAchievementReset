#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

#include "vcall.h"

#define INTERFACEVERSION_GAMEEVENTSMANAGER2 "GAMEEVENTSMANAGER002"

typedef abstract_class IGameEvent *IGameEvent;
typedef abstract_class IGameEventListener2 *IGameEventListener2;
typedef abstract_class IGameEventManager2 *IGameEventManager2;
typedef struct CGameEventListener CGameEventListener;

abstract_class IGameEvent {
    void (*virtual dtor)(void *this);
    const char *(*virtual GetName)(void *this);
    bool (*virtual IsReliable)(void *this);
    bool (*virtual IsLocal)(void *this);
    bool (*virtual IsEmpty)(void *this, const char *keyName);
    bool (*virtual GetBool)(void *this, const char *keyName, bool defaultValue);
    int (*virtual GetInt)(void *this, const char *keyName, int defaultValue);
    float (*virtual GetFloat)(void *this, const char *keyName,
                              float defaultValue);
    const char *(*virtual GetString)(void *this, const char *keyName,
                                     const char *defaultValue);
    void (*virtual SetBool)(void *this, const char *keyName, bool value);
    void (*virtual SetInt)(void *this, const char *keyName, int value);
    void (*virtual SetFloat)(void *this, const char *keyName, float value);
    void (*virtual SetString)(void *this, const char *keyName,
                              const char *value);
};

abstract_class IGameEventListener2 {
    void (*virtual dtor)(void *this);
    void (*virtual FireGameEvent)(void *this, IGameEvent *event);
};

abstract_class IGameEventManager2 {
    void (*virtual dtor)(void *this);
    int (*virtual LoadEventsFromFile)(void *this, const char *filename);
    void (*virtual Reset)(void *this);
    bool (*virtual AddListener)(void *this, IGameEventListener2 *listener,
                                const char *name, bool bServerSide);
    bool (*virtual FindListener)(void *this, IGameEventListener2 *listener,
                                 const char *name);
    void (*virtual RemoveListener)(void *this, IGameEventListener2 *listener);
    IGameEvent *(*virtual CreateEvent)(void *this, const char *name,
                                       bool bForce);
    bool (*virtual FireEvent)(void *this, IGameEvent *event,
                              bool bDontBroadcast);
    bool (*virtual FireEventClientSide)(void *this, IGameEvent *event);
    IGameEvent *(*virtual DuplicateEvent)(void *this, IGameEvent *event);
    void (*virtual FreeEvent)(void *this, IGameEvent *event);
    bool (*virtual SerializeEvent)(void *this, IGameEvent *event, void *buf);
    IGameEvent *(*virtual UnserializeEvent)(void *this, void *buf);
};

struct CGameEventListener {
    IGameEventListener2 base;
    bool registered_for_events;
};

extern IGameEventManager2 *game_event_mgr;

static inline void ListenForGameEvent(CGameEventListener *this,
                                      const char *name, bool server_side) {
    this->registered_for_events = true;
    (*game_event_mgr)
        ->AddListener(game_event_mgr, &(this->base), name, server_side);
}

static inline void StopListeningForAllEvents(CGameEventListener *this) {
    if (this->registered_for_events) {
        (*game_event_mgr)->RemoveListener(game_event_mgr, &(this->base));
        this->registered_for_events = false;
    }
}

#endif
