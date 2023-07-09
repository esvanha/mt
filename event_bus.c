#include "event_bus.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

EventBus
event_bus_new(void)
{
    return (EventBus){
        .front = NULL,
        .rear = NULL
    };
}

/**
 * @brief 
 * 
 * @param event_bus 
 * @param event_message 
 * @note The poller is responsible for freeing the message after use.
 * @return true 
 * @return false 
 */
bool
event_bus_poll(EventBus* event_bus, EventMessage** event_message)
{
    if (event_bus->front == NULL)
    {
        return false;
    }

    *event_message = event_bus->front->message;

    EventBusItem* old_front = event_bus->front;
    event_bus->front = event_bus->front->next;
    SAFE_FREE(old_front);

    if (event_bus->front == NULL)
    {
        event_bus->rear = NULL;
    }

    return true;
}

void
event_bus_notify(EventBus* event_bus, EventMessage event_message)
{
    EventBusItem* event_bus_item = malloc(sizeof(EventBusItem));
    if (event_bus_item == NULL)
    {
        // TODO: error returnen
        assert(false);
    }

    event_bus_item->message = malloc(sizeof(EventMessage));
    if (event_bus_item->message == NULL)
    {
        // TODO: error returnen
        assert(false);
    }
    memcpy(event_bus_item->message, &event_message, sizeof(EventMessage));
    event_bus_item->next = NULL;

    if (event_bus->front == NULL && event_bus->rear == NULL)
    {
        event_bus->front = event_bus->rear = event_bus_item;
    }
    else
    {
        event_bus->rear->next = event_bus_item;
        event_bus->rear = event_bus_item;
    }
}

void
event_bus_free(EventBus* event_bus)
{
    while (event_bus_poll(event_bus, NULL)) {}
    assert(event_bus->front == NULL);
    assert(event_bus->rear == NULL);
}
