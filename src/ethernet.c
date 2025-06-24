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
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_DISABLED));
    ESP_LOGI(TAG, "Step 5 done");

    ESP_LOGI(TAG, "Step 6: spi_bus_add_device");
    // Changed to same as SPI test code that worked.
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000, // Lowered for debug
        .mode = 0,
        .spics_io_num = W5500_CS,
        .command_bits = 0,
        .queue_size = 1,
        .address_bits = 0
    };
    spi_device_handle_t spi_handle;
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle));
    vTaskDelay(pdMS_TO_TICKS(250));
    ESP_LOGI(TAG, "Step 6 done");
/*
    ESP_LOGI(TAG, "Step 6a: manually check SPI");
     // Compose the command frame
    uint8_t cmd_buf[3] = { 0x00, 0x39, 0x00 };  // Address + control byte
    uint8_t rx_data = 0;

    spi_transaction_t t = {
        .length = 8 * 4,
        .tx_buffer = cmd_buf,
        .rx_buffer = &rx_data,
        .flags = SPI_TRANS_USE_RXDATA,
    };

    // Pad rx buffer with 1 dummy byte so we read the 4th byte correctly
    uint8_t tx_rx_buf[4] = { 0x00, 0x39, 0x00, 0x00 };
    memset(&t, 0, sizeof(t));
    t.length = 8 * 4;
    t.tx_buffer = tx_rx_buf;
    t.rx_buffer = tx_rx_buf;

    ESP_LOGI(TAG, "Reading version register...");
    esp_err_t ret;
    ret = spi_device_transmit(spi_handle, &t);
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "W5500 Version Register: 0x%02X", tx_rx_buf[3]);   
    ESP_LOGI(TAG, "Step 6a done");
*/
    ESP_LOGI(TAG, "Step 7: ETH_W5500_DEFAULT_CONFIG");
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.reset_gpio_num = -1;
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(SPI2_HOST, &devcfg);
    ESP_LOGI(TAG, "Step 7 done");

    ESP_LOGI(TAG, "Step 8: esp_eth_mac_new_w5500");
    esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);

    // Set a default MAC address
    uint8_t default_mac[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01}; // Example MAC address
    if (mac && mac->set_addr) {
        ESP_ERROR_CHECK(mac->set_addr(mac, default_mac));
        ESP_LOGI(TAG, "Default MAC address set: %02X:%02X:%02X:%02X:%02X:%02X", 
                 default_mac[0], default_mac[1], default_mac[2], 
                 default_mac[3], default_mac[4], default_mac[5]);
    }

    // Retrieve and log the MAC address to confirm it was set
    uint8_t mac_addr[6];
    if (mac && mac->get_addr && mac->get_addr(mac, mac_addr) == ESP_OK) {
        ESP_LOGI(TAG, "MAC address in use: %02X:%02X:%02X:%02X:%02X:%02X", 
                 mac_addr[0], mac_addr[1], mac_addr[2], 
                 mac_addr[3], mac_addr[4], mac_addr[5]);
    } else {
        ESP_LOGE(TAG, "Failed to get MAC address");
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
