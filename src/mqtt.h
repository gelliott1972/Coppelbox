/**
 * @file mqtt.h
 * @brief MQTT client header
 */

#ifndef MQTT_H
#define MQTT_H

#include <stdbool.h>

void mqtt_init(void);
void mqtt_publish_relay_state(int relay, bool state);

#endif // MQTT_H
