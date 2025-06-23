/**
 * @file mqtt.c
 * @brief MQTT client implementation for Copplebox_3
 */

#include "mqtt.h"
#include "copplebox.h"
#include "esp_log.h"
#include "mqtt_client.h"

static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t client = NULL;

void mqtt_publish_relay_state(int relay, bool state) {
    if (!client) return;

    char topic[64];
    snprintf(topic, sizeof(topic), "copplebox/relay/%d", relay);

    char message[8];
    snprintf(message, sizeof(message), "%s", state ? "ON" : "OFF");

    esp_mqtt_client_publish(client, topic, message, 0, 1, 0);
}

void mqtt_init(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_SERVER_URI,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
    ESP_LOGI(TAG, "MQTT client started.");
}
