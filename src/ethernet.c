/**
 * @file ethernet.c
 * @brief Ethernet initialization for Copplebox_3
 */

#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "ethernet.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include <string.h>

static const char *TAG = "ETHERNET";

static esp_netif_t *eth_netif = NULL;

void ethernet_init(void)
{
    ESP_LOGI(TAG, "Initializing Ethernet...");
    esp_netif_init();
    esp_event_loop_create_default();
    eth_netif = esp_netif_create_default_eth_netif();
    assert(eth_netif);

    esp_eth_handle_t eth_handle = NULL;
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    mac_config.smi_mdc_gpio_num = CONFIG_ETH_MDC_GPIO;
    mac_config.smi_mdio_gpio_num = CONFIG_ETH_MDIO_GPIO;

    eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    ESP_LOGI(TAG, "Ethernet initialized.");
}
