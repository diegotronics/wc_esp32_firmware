/*
 * wc_esp32_udp.h
 *
 *  Created on: 14 ago. 2018
 *      Author: Diego Perez
 */
#ifndef WC_ESP32_UDP_H_
#define WC_ESP32_UDP_H_

#include "wc_esp32_common.h"
#include "wc_esp32_spi.h"
#include "wc_esp32_wifi.h"
#include "wc_esp32_mqtt.h"

/* DEFINES */
// UDP Socket
#define SERVER_ADDRESS		 	"192.168.0.94"
#define SOCKET_PORT_UDP_TX	 	4000
// TAG
#define TAG_SOCKET "UDP"

/* GLOBAL VARIABLES */
struct sockaddr_in server_address;
int sock_udp_tx;
struct sockaddr_in udp_tx_address;


void socket_udp_cfg(void);
void IRAM_ATTR socket_udp_send(void *void_parameter);

#endif /* WC_ESP32_UDP_H_ */
