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
#include <assert.h>
#include "copplexbox.h"

static const char *TAG = "ETHERNET";

static esp_netif_t *eth_netif = NULL;

void ethernet_init(void)
{
    ESP_LOGI(TAG, "Initializing Ethernet...");
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_inherent_config_t cfg = ESP_NETIF_INHERENT_DEFAULT_ETH();
    esp_netif_config_t netif_cfg = {
        .base = &cfg,
        .driver = NULL,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };
    eth_netif = esp_netif_new(&netif_cfg);
    assert(eth_netif);

    ESP_LOGI(TAG, "Ethernet netif created.");

    esp_eth_handle_t eth_handle = NULL;
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    eth_esp32_emac_config_t emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "Ethernet netif created.");

    emac_config.smi_mdc_gpio_num = EMAC_MDC_GPIO;
    emac_config.smi_mdio_gpio_num = EMAC_MDIO_GPIO;

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&emac_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);

    ESP_LOGI(TAG, "Ethernet mac created.");

    ESP_LOGI(TAG, "MAC: %p, PHY: %p", mac, phy);
    assert(mac != NULL);
    assert(phy != NULL);

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_err_t err = esp_eth_driver_install(&config, &eth_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install Ethernet driver: %s", esp_err_to_name(err));
        return;
    }
    err = esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to attach netif: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Ethernet phy created.");

    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    ESP_LOGI(TAG, "Ethernet initialized.");
}
