/**
 * @file buttons.c
 * @brief Button handler implementation using ADC and polling
 */

#include "buttons.h"
#include "copplebox.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include <stdbool.h>

#define INVALID -1
#define ADC_WIDTH ADC_BITWIDTH_12
#define ADC_ATTEN ADC_ATTEN_DB_11

static const char *TAG = "BUTTONS";
static bool button_state[BUTTON_COUNT] = {false, false, false};

// ADC oneshot driver handle
static adc_oneshot_unit_handle_t adc_handle;
static adc_channel_t adc_channel;

void buttons_init(void) {
    ESP_LOGI(TAG, "Initializing ADC on GPIO %d", BUTTON_ADC_GPIO);

    // Configure ADC unit
    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

    // Determine the ADC channel based on the GPIO
    switch (BUTTON_ADC_GPIO) {
        case 36: adc_channel = ADC_CHANNEL_0; break;
        case 37: adc_channel = ADC_CHANNEL_1; break;
        case 38: adc_channel = ADC_CHANNEL_2; break;
        case 39: adc_channel = ADC_CHANNEL_3; break;
        case 32: adc_channel = ADC_CHANNEL_4; break;
        case 33: adc_channel = ADC_CHANNEL_5; break;
        case 34: adc_channel = ADC_CHANNEL_6; break;
        case 35: adc_channel = ADC_CHANNEL_7; break;
        default:
            ESP_LOGE(TAG, "Invalid ADC1 GPIO: %d", BUTTON_ADC_GPIO);
            return;
    }

    // Configure the ADC channel
    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_WIDTH,
        .atten = ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, adc_channel, &chan_cfg));
}

static int read_adc(void) {
    int adc_val = 0;
    esp_err_t err = adc_oneshot_read(adc_handle, adc_channel, &adc_val);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read ADC value: %s", esp_err_to_name(err));
        return -1;
    }
    return adc_val;
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
