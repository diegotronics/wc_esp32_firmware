#include "wc_esp32_udp.h"

DRAM_ATTR char socket_data_to_server_udp[QUEUE_TO_SERVER_UDP_LENGTH]="";

/* SOCKET UDP SEND CONFIGURATION */
void socket_udp_cfg(void){
	while(false == connected){
		vTaskDelay(2000 / portTICK_RATE_MS);
	}
	sock_udp_tx = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock_udp_tx < 0) {
		ESP_LOGI(TAG_SOCKET, "CREATE SOCKET STATUS: %s", strerror(errno));
		close(sock_udp_tx);
	}

	udp_tx_address.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_ADDRESS, &udp_tx_address.sin_addr.s_addr);
	udp_tx_address.sin_port = htons(SOCKET_PORT_UDP_TX);

	ESP_LOGI(TAG_SOCKET, "Socket UDP Ready");

	xTaskCreate(&socket_udp_send, "socket_udp_send", 1024, NULL, 3, NULL);
}

/* SOCKET TO SEND MULTIMEDIA OVER UDP*/
IRAM_ATTR void socket_udp_send(void *void_parameter){
	const TickType_t xBlockTime = pdMS_TO_TICKS( 10 );
	char end_queue_tx[3] = "EOT";
	while (1) {
		if (xQueue_to_server_udp!=0){
			if(xQueueReceive(xQueue_to_server_udp, &socket_data_to_server_udp, xBlockTime)==pdTRUE){
				if (strcmp(socket_data_to_server_udp, end_queue_tx) == 0){
					sendto(sock_udp_tx, end_queue_tx, sizeof(end_queue_tx), 0, (struct sockaddr *)&udp_tx_address, sizeof(udp_tx_address));
					vTaskDelay(xBlockTime);
				}
				else{
					sendto(sock_udp_tx, socket_data_to_server_udp, sizeof(socket_data_to_server_udp), 0, (struct sockaddr *)&udp_tx_address, sizeof(udp_tx_address));
				}
			}
		}
	}
}

