/**
 * @file main.c
 * @brief Entry point for Copplebox_3 project
 */

#include <stdio.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "copplexbox.h"
#include "relays.h"
#include "buttons.h"
#include "ethernet.h"
#include "mqtt.h"
#include "esp_idf_version.h"

static const char *TAG = "MAIN";

typedef enum {
    CYCLING_UP,
    CYCLING_DOWN,
    PAUSED
} system_state_t;

volatile bool flag_1s = false;
volatile bool flag_30ms = false;

static system_state_t current_state = CYCLING_UP;
static int current_relay = 0;

void timer_1s_callback(void* arg) { flag_1s = true; }
void timer_30ms_callback(void* arg) { flag_30ms = true; }

void app_main(void)
{
    printf("Welcome to Copplebox.\n");
    printf("Version 3.0\n");
    printf("ESP-IDF Version: %s\n", esp_get_idf_version());

    relays_init();
    ethernet_init();
    mqtt_init();
    buttons_init();

    const esp_timer_create_args_t timer_1s_args = {
        .callback = &timer_1s_callback
    };
    const esp_timer_create_args_t timer_30ms_args = {
        .callback = &timer_30ms_callback
    };

    esp_timer_handle_t timer_1s;
    esp_timer_handle_t timer_30ms;

    esp_timer_create(&timer_1s_args, &timer_1s);
    esp_timer_create(&timer_30ms_args, &timer_30ms);
    esp_timer_start_periodic(timer_1s, 1000000);
    esp_timer_start_periodic(timer_30ms, 30000);

    while (true) {
        if (flag_30ms) {
            flag_30ms = false;
            button_event_t event;
            if (buttons_poll(&event)) {
                ESP_LOGI(TAG, "Button event: %s - %s",
                         button_name(event.button),
                         event.pressed ? "PRESSED" : "RELEASED");

                if (event.pressed) {
                    switch (event.button) {
                        case BUTTON_UP: current_state = CYCLING_UP; break;
                        case BUTTON_DOWN: current_state = CYCLING_DOWN; break;
                        case BUTTON_PAUSE: current_state = PAUSED; break;
                    }
                }
            }
        }

        if (flag_1s) {
            flag_1s = false;

            if (current_state != PAUSED) {
                int next_relay = (current_state == CYCLING_UP)
                                 ? (current_relay + 1) % NUMBER_OF_RELAYS
                                 : (current_relay - 1 + NUMBER_OF_RELAYS) % NUMBER_OF_RELAYS;
                relays_set(current_relay, false);
                mqtt_publish_relay_state(current_relay, false);
                relays_set(next_relay, true);
                mqtt_publish_relay_state(next_relay, true);
                current_relay = next_relay;
            }
        }
    }
}
