/**
 * @file buttons.h
 * @brief Button handler API
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>

typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_PAUSE
} button_t;

typedef struct {
    button_t button;
    bool pressed;
} button_event_t;

void buttons_init(void);
bool buttons_poll(button_event_t *event);
const char* button_name(button_t button);

#endif // BUTTONS_H
