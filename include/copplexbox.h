#ifndef COPPLEXBOX_H
#define COPPLEXBOX_H

// Relay GPIOs
#define NUMBER_OF_RELAYS 4
#define RELAY_0_GPIO     12
#define RELAY_1_GPIO     2
#define RELAY_2_GPIO     27
#define RELAY_3_GPIO     4

// Button ADC
#define BUTTON_ADC_GPIO  36
#define BUTTON_COUNT     3

// Nominal ADC values
#define BUTTON_UP_VALUE      1900
#define BUTTON_DOWN_VALUE    800
#define BUTTON_PAUSE_VALUE   350
#define BUTTON_TOLERANCE     50

// MQTT server IP
#define MQTT_SERVER_URI "mqtt://192.168.50.168"

// EMAC (Ethernet MAC) GPIOs
#define EMAC_MDC_GPIO 23  // Replace with your actual MDC GPIO
#define EMAC_MDIO_GPIO 18 // Replace with your actual MDIO GPIO

#endif // COPPLEXBOX_H
