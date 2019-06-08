/*
 * spi.h
 *
 *  Created on: 10 ago. 2018
 *      Author: -
 */

#ifndef MAIN_WC_ESP32_SPI_H_
#define MAIN_WC_ESP32_SPI_H_

/* INCLUDES */
#include "nvs_flash.h"
#include "driver/spi_slave.h"

#include "esp_spi_flash.h"
#include "esp_heap_caps.h"

#include "wc_esp32_udp.h"
#include "wc_esp32_wifi.h"
#include "wc_esp32_mqtt.h"
#include "wc_esp32_common.h"

/* DEFINES */
// GPIO to SPI
#define GPIO_SPI_HANDSHAKE	2
#define GPIO_SPI_MOSI 		13
#define GPIO_SPI_MISO 		12
#define GPIO_SPI_SCLK 		14
#define GPIO_SPI_CS 		15
#define GPIO_SPI_WAKEUP		27

// SPI cfg
#define SPI_DMA_CHANNEL		1
#define SPI_MODE			0
#define SPI_QUEUE_SIZE		1
#define SPI_FLAGS			0

// Commands from camera
#define CAMERA_HEADER		35
#define CAMERA_ERROR		0
#define CAMERA_ACK			6
#define CAMERA_WAKEUP 		10
#define CAMERA_ESP32_STATUS	11
#define CAMERA_TX_IMAGE		12
#define CAMERA_TX_AUDIO		13
#define CAMERA_TO_SERVER	14
#define CAMERA_ID			15

// Commands from ESP32
#define ESP32_HEADER		35
#define ESP32_ACK			6
#define ESP32_ASK_ID		20
#define ESP32_NAK			21
#define ESP32_DISCONNECTED	65
#define ESP32_CONNECTED		66


#define QUEUE_TO_SERVER_UDP_LENGTH	1024
#define QUEUE_COMMAND_LENGTH		32
#define SIZE_DATA_TO_CAMERA			32
#define QUEUE_BUFFER_SIZE			3
#define QUEUE_TO_CAMERA_TCP_LENGTH	32
// TAG
#define TAG_SPI "SPI"

static const TickType_t xBlockTime = pdMS_TO_TICKS(100);
/* GLOBAL VARIABLES */
extern QueueHandle_t xQueue_to_camera_tcp;
extern QueueHandle_t xQueue_to_server_udp;
esp_err_t ret;

/* FUNCTIONS */
void my_post_setup_cb(spi_slave_transaction_t *trans);
void my_post_trans_cb(spi_slave_transaction_t *trans);
void spi_wakeup_high(void);
void spi_wakeup_low(void);
void spi_cfg(void);
IRAM_ATTR void spi_receive(void *void_parameters);
spi_slave_transaction_t spi_transaction(char *tx, char *rx, int length);
void spi_to_camera(char *data_to_camera);

#endif /* MAIN_WC_ESP32_SPI_H_ */
