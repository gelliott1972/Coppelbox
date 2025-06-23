/**
 * @file relays.c
 * @brief Implementation of relay control
 */

#include "relays.h"
#include "copplebox.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdbool.h>

static const char *TAG = "RELAYS";

static const int relay_pins[NUMBER_OF_RELAYS] = {
    RELAY_0_GPIO, RELAY_1_GPIO, RELAY_2_GPIO, RELAY_3_GPIO
};

void relays_init(void) {
    for (int i = 0; i < NUMBER_OF_RELAYS; i++) {
        gpio_reset_pin(relay_pins[i]);
        gpio_set_direction(relay_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(relay_pins[i], 0);
    }
}

void relays_set(int relay, bool state) {
    if (relay < 0 || relay >= NUMBER_OF_RELAYS) return;
    gpio_set_level(relay_pins[relay], state);
    ESP_LOGI(TAG, "Relay %d -> %s", relay, state ? "ON" : "OFF");
}
