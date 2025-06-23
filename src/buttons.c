/**
 * @file buttons.c
 * @brief Button handler implementation using ADC and polling
 */

#include "buttons.h"
#include "copplexbox.h"
#include "driver/adc.h"
#include "esp_log.h"
#include <stdbool.h>

#define INVALID -1
#define ADC_WIDTH ADC_WIDTH_BIT_12
#define ADC_ATTEN ADC_ATTEN_DB_11

static const char *TAG = "BUTTONS";
static bool button_state[BUTTON_COUNT] = {false, false, false};

void buttons_init(void) {
    ESP_LOGI(TAG, "Initializing ADC on GPIO %d", BUTTON_ADC_GPIO);

    adc1_config_width(ADC_WIDTH);
    adc1_channel_t channel;

    switch (BUTTON_ADC_GPIO) {
        case 36: channel = ADC1_CHANNEL_0; break;
        case 37: channel = ADC1_CHANNEL_1; break;
        case 38: channel = ADC1_CHANNEL_2; break;
        case 39: channel = ADC1_CHANNEL_3; break;
        case 32: channel = ADC1_CHANNEL_4; break;
        case 33: channel = ADC1_CHANNEL_5; break;
        case 34: channel = ADC1_CHANNEL_6; break;
        case 35: channel = ADC1_CHANNEL_7; break;
        default:
            ESP_LOGE(TAG, "Invalid ADC1 GPIO: %d", BUTTON_ADC_GPIO);
            return;
    }

    adc1_config_channel_atten(channel, ADC_ATTEN);
}

static int read_adc(void) {
    adc1_channel_t channel;
    switch (BUTTON_ADC_GPIO) {
        case 36: channel = ADC1_CHANNEL_0; break;
        case 37: channel = ADC1_CHANNEL_1; break;
        case 38: channel = ADC1_CHANNEL_2; break;
        case 39: channel = ADC1_CHANNEL_3; break;
        case 32: channel = ADC1_CHANNEL_4; break;
        case 33: channel = ADC1_CHANNEL_5; break;
        case 34: channel = ADC1_CHANNEL_6; break;
        case 35: channel = ADC1_CHANNEL_7; break;
        default: return -1;
    }

    return adc1_get_raw(channel);
}

static int detect_button(int adc_val) {
    if (adc_val >= BUTTON_UP_VALUE - BUTTON_TOLERANCE &&
        adc_val <= BUTTON_UP_VALUE + BUTTON_TOLERANCE)
        return BUTTON_UP;

    if (adc_val >= BUTTON_DOWN_VALUE - BUTTON_TOLERANCE &&
        adc_val <= BUTTON_DOWN_VALUE + BUTTON_TOLERANCE)
        return BUTTON_DOWN;

    if (adc_val >= BUTTON_PAUSE_VALUE - BUTTON_TOLERANCE &&
        adc_val <= BUTTON_PAUSE_VALUE + BUTTON_TOLERANCE)
        return BUTTON_PAUSE;

    return INVALID;
}

bool buttons_poll(button_event_t *event) {
    int adc_val = read_adc();
    int detected = detect_button(adc_val);

    for (int i = 0; i < BUTTON_COUNT; i++) {
        bool pressed = (i == detected);
        if (pressed != button_state[i]) {
            button_state[i] = pressed;
            if (event) {
                event->button = (button_t)i;
                event->pressed = pressed;
                return true;
            }
        }
    }
    return false;
}

const char* button_name(button_t button) {
    switch (button) {
        case BUTTON_UP: return "button_up";
        case BUTTON_DOWN: return "button_down";
        case BUTTON_PAUSE: return "button_pause";
        default: return "unknown";
    }
}
