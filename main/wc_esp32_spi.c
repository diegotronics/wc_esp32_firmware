/*
 * spi.c
 *
 *  Created on: 10 ago. 2018
 *      Author: Diego Perez
 */
#include "wc_esp32_spi.h"


bool send_data = false;

// Transaction for SPI transmit
DRAM_ATTR char spi_data_to_server[QUEUE_TO_SERVER_UDP_LENGTH]="";
DRAM_ATTR char spi_data_command[QUEUE_COMMAND_LENGTH]="";
DRAM_ATTR char spi_tx_buffer[32]="";
DRAM_ATTR QueueHandle_t xQueue_to_server_udp = NULL;

DRAM_ATTR char spi_data_to_camera_tcp[QUEUE_TO_CAMERA_TCP_LENGTH]="";


/*	SET HANDSHAKE LINE HIGH */
void my_post_setup_cb(spi_slave_transaction_t *trans) {
    WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1<<GPIO_SPI_HANDSHAKE));
}

/*	SET HANDSHAKE LINE LOW */
void my_post_trans_cb(spi_slave_transaction_t *trans) {
    WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1<<GPIO_SPI_HANDSHAKE));
}

/*	SET WAKEUP LINE HIGH */
void spi_wakeup_high(void) {
    WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1<<GPIO_SPI_WAKEUP));
}

/*	SET WAKEUP LINE LOW */
void spi_wakeup_low(void) {
    WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1<<GPIO_SPI_WAKEUP));
}

/* SPI PORT CONFIGURATION */
void spi_cfg(void){

    //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_SPI_MOSI,
        .miso_io_num = GPIO_SPI_MISO,
        .sclk_io_num = GPIO_SPI_SCLK
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .mode = SPI_MODE,
        .queue_size = SPI_QUEUE_SIZE,
        .flags = SPI_FLAGS,
		.spics_io_num = GPIO_SPI_CS,
		.post_setup_cb = my_post_setup_cb,
		.post_trans_cb = my_post_trans_cb
    };

    //Configuration for the handshake line
    gpio_config_t io_conf_handshake={
   		.intr_type=GPIO_INTR_DISABLE,
		.mode=GPIO_MODE_OUTPUT,
		.pin_bit_mask=(1<<GPIO_SPI_HANDSHAKE)
    };
    //Configure handshake line as output
    gpio_config(&io_conf_handshake);

    //Configuration for the wakeup line
    gpio_config_t io_conf_wakeup={
   		.intr_type=GPIO_INTR_DISABLE,
		.mode=GPIO_MODE_OUTPUT,
		.pin_bit_mask=(1<<GPIO_SPI_WAKEUP)
    };
    //Configure the wakeup line as output
    gpio_config(&io_conf_wakeup);
    spi_wakeup_low();

    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_SPI_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SPI_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SPI_CS, GPIO_PULLUP_ONLY);

    //Initialize SPI slave interface
    ret = spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, SPI_DMA_CHANNEL);
    ESP_LOGI(TAG_SPI, "SPI Ready");
    xTaskCreate(&spi_receive, "spi_receive", 2048, NULL, 3, NULL);
}

/* SPI TRANSACTION: SEND AND RECEIVE */
IRAM_ATTR void spi_receive(void *void_parameters){
	xQueue_to_server_udp = xQueueCreate(10, QUEUE_TO_SERVER_UDP_LENGTH);
	spi_slave_transaction_t upp_t;
	int *id = spi_data_command;
	char camera_id[25];

	while (1){
		spi_transaction(NULL, spi_data_command, 32);
		int i;
		for(i=0; i<32;i++){
			printf("%d ", spi_data_command[i]);
		}
		printf("\n");
		if (CAMERA_HEADER == spi_data_command[0]){
			switch(spi_data_command[1]){
				case CAMERA_WAKEUP:
					spi_data_command[0] = ESP32_HEADER;
					spi_data_command[1] = ESP32_ACK;
					spi_transaction(spi_data_command, NULL, 32);

					send_data = true;
					while(send_data == true){
						vTaskDelay(2000 / portTICK_RATE_MS);
					}

					break;

				case CAMERA_ESP32_STATUS:
					spi_data_command[0] = ESP32_HEADER;
					spi_data_command[1] = ESP32_ACK;
					spi_transaction(spi_data_command, NULL, 32);

					memset(&spi_tx_buffer, 0, sizeof(spi_tx_buffer));
					spi_tx_buffer[0] = ESP32_HEADER;
					if(connected){
						spi_tx_buffer[1] = ESP32_CONNECTED;
					}
					else{
						spi_tx_buffer[1] = ESP32_DISCONNECTED;
					}
					spi_transaction(spi_tx_buffer, NULL, sizeof(spi_tx_buffer));
					break;

				case CAMERA_TX_IMAGE:
					spi_data_command[0] = ESP32_HEADER;
					spi_data_command[1] = ESP32_ACK;
					spi_transaction(spi_data_command, NULL, 32);

					memset(&spi_data_to_server, 0, sizeof(spi_data_to_server));

					while(1){
						 = spi_transaction(NULL, spi_data_to_server, 1028);

						if(t.trans_len/8 == 32){
							snprintf(spi_data_to_server, QUEUE_TO_SERVER_UDP_LENGTH, "EOT");
							xQueueSend(xQueue_to_server_udp, &spi_data_to_server, xBlockTime);
							break;
						}
						else {
							xQueueSend(xQueue_to_server_udp, &spi_data_to_server, xBlockTime);
						}
					}
					break;

				case CAMERA_TX_AUDIO:
					spi_data_command[0] = ESP32_HEADER;
					spi_data_command[1] = ESP32_ACK;
					spi_transaction(spi_data_command, NULL, 32);

					break;

				case CAMERA_TO_SERVER:
					spi_data_command[0] = ESP32_HEADER;
					spi_data_command[1] = ESP32_ACK;
					spi_transaction(spi_data_command, NULL, 32);

					//PUB VIA MQTT
					break;

				case CAMERA_ID:
					spi_data_command[0] = ESP32_HEADER;
					spi_data_command[1] = ESP32_ACK;
					spi_transaction(spi_data_command, NULL, 32);

					spi_transaction(NULL, spi_data_command, 32);
					sprintf(camera_id, "%08x%08x%08x", id[2], id[1], id[0]);
					mqtt_pub_id(camera_id);
					mqtt_sub_id(camera_id);

					spi_data_command[0] = ESP32_HEADER;
					spi_data_command[1] = ESP32_ACK;
					spi_transaction(spi_data_command, NULL, 32);

					break;

				default:
					break;
			}
		}

	}
}


spi_slave_transaction_t spi_transaction(char *tx, char *rx, int length){
	spi_slave_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.tx_buffer = tx;
	t.rx_buffer = rx;
	t.length = length*8;
	spi_slave_transmit(HSPI_HOST, &t, portMAX_DELAY);
	return t;
}

void spi_to_camera(char *data_to_camera){
    spi_wakeup_high();
    vTaskDelay(100 / portTICK_RATE_MS);
    spi_wakeup_low();

	while(false == send_data){
		vTaskDelay(2000 / portTICK_RATE_MS);
	}

	sprintf(spi_data_command, "%s", data_to_camera);
	spi_transaction(spi_data_command, NULL, 32);

	ESP_LOGI(TAG_SPI, "Data Sent: %s", data_to_camera);
	send_data = false;
}
