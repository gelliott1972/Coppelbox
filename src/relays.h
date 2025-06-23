/**
 * @file relays.h
 * @brief Relay control API
 */

#ifndef RELAYS_H
#define RELAYS_H

#include <stdbool.h>

void relays_init(void);
void relays_set(int relay, bool state);

#endif // RELAYS_H
