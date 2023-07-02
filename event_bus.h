#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <stdbool.h>

typedef struct {
    float hz;
    float amplitude;
} EventNoteEnable;

enum EventMessageType {
    EVENT_NOTE_ENABLE
};

typedef struct {
    enum EventMessageType type;
    union {
        EventNoteEnable note_enable;
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

EventBus event_bus_new();
bool event_bus_poll(EventBus* event_bus, EventMessage** event_message);
void event_bus_notify(EventBus* event_bus, EventMessage event_message);
void event_bus_free(EventBus* event_bus);

#endif
