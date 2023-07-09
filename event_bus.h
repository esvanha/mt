#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <stdbool.h>

typedef struct {
    float hz;
    float amplitude;
} EventNoteEnable;

typedef struct {
    float hz;
} EventNoteDisable;

enum EventMessageType {
    EVENT_NOTE_ENABLE,
    EVENT_NOTE_DISABLE
};

typedef struct {
    enum EventMessageType type;
    union {
        EventNoteEnable note_enable;
        EventNoteDisable note_disable;
    } event;
} EventMessage;

typedef struct EventBusItem {
    EventMessage* message;
    struct EventBusItem* next;
} EventBusItem;

typedef struct {
    EventBusItem* front;
    EventBusItem* rear;
} EventBus;

EventBus event_bus_new(void);
bool event_bus_poll(EventBus* event_bus, EventMessage** event_message);
void event_bus_notify(EventBus* event_bus, EventMessage event_message);
void event_bus_free(EventBus* event_bus);

#endif
