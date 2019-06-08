/*
 * wc_esp32_main.h
 *
 *  Created on: 20 sept. 2018
 *      Author: Diego Perez
 *      email: diegoperez1295@gmail.com
 */
#include "wc_esp32_common.h"
#include "wc_esp32_spi.h"
#include "wc_esp32_wifi.h"
#include "wc_esp32_nvs.h"
#include "wc_esp32_mqtt.h"
#include "wc_esp32_udp.h"

bool connected = false;

/* MAIN */
void app_main(void){
	connected = false;
	nvs_cfg();
	wifi_cfg();
    socket_udp_cfg();
    mqtt_cfg();
	spi_cfg();
}
