/**
 * @file ethernet.c
 * @brief W5500 Ethernet initialization for Copplebox_3
 */
#include <string.h>
#include <assert.h>

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_eth_driver.h"


#include "ethernet.h"
#include "copplebox.h"

static const char *TAG = "ETH_W5500";

void ethernet_init(void)
{
    ESP_LOGI(TAG, "Initializing W5500 Ethernet...");

    ESP_LOGI(TAG, "Step 1: esp_netif_init");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_LOGI(TAG, "Step 1 done");

    ESP_LOGI(TAG, "Step 2: esp_event_loop_create_default");
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI(TAG, "Step 2 done");

    ESP_LOGI(TAG, "Step 3: gpio_install_isr_service");
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_LOGI(TAG, "Step 3 done");

    ESP_LOGI(TAG, "Step 4: esp_netif_new");
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&cfg);
    ESP_LOGI(TAG, "Step 4 done");

    ESP_LOGI(TAG, "Step 5: spi_bus_initialize");
    spi_bus_config_t buscfg = {
        .miso_io_num = W5500_MISO,
        .mosi_io_num = W5500_MOSI,
        .sclk_io_num = W5500_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_LOGI(TAG, "Step 5 done");

    ESP_LOGI(TAG, "Step 6: spi_bus_add_device");
    spi_device_interface_config_t devcfg = {
        .command_bits = 16,
        .address_bits = 8,
        .mode = 0,
        .clock_speed_hz = 1 * 1000 * 1000, // Lowered for debug
        .spics_io_num = W5500_CS,
        .queue_size = 20
    };
    spi_device_handle_t spi_handle;
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle));
    ESP_LOGI(TAG, "Step 6 done");

    ESP_LOGI(TAG, "Step 7: ETH_W5500_DEFAULT_CONFIG");
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(SPI2_HOST, &devcfg);
    ESP_LOGI(TAG, "Step 7 done");

    ESP_LOGI(TAG, "Step 8: esp_eth_mac_new_w5500");
    esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
    uint8_t mac_addr[6];
    if (mac && mac->get_addr && mac->get_addr(mac, mac_addr) == ESP_OK) {
        ESP_LOGI(TAG, "MAC address: %02X:%02X:%02X:%02X:%02X:%02X", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    } else {
        ESP_LOGI(TAG, "Failed to get MAC address");
    }
    ESP_LOGI(TAG, "Step 8 done");

    ESP_LOGI(TAG, "Step 9: esp_eth_phy_new_w5500");
    esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);
    ESP_LOGI(TAG, "Step 9 done");

    ESP_LOGI(TAG, "Step 10: esp_eth_driver_install");
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config, &eth_handle));
    ESP_LOGI(TAG, "Step 10 done");

    ESP_LOGI(TAG, "Step 11: esp_netif_attach");
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));
    ESP_LOGI(TAG, "Step 11 done");

    ESP_LOGI(TAG, "Step 12: esp_eth_start");
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    ESP_LOGI(TAG, "Step 12 done");

    ESP_LOGI(TAG, "W5500 Ethernet started.");
}
