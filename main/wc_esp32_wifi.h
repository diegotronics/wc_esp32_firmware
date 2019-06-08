/*
 * wc_esp32_wifi.h
 *
 *  Created on: 14 ago. 2018
 *      Author: -
 */

#ifndef WIFI_H_
#define WIFI_H_

#include "wc_esp32_common.h"
#include "wc_esp32_nvs.h"
#include "wc_esp32_spi.h"
#include "wc_esp32_udp.h"

#define GPIO_WIFI_CLEAR_CREDENTIALS		0
#define WIFI_SMARTCONFIG_TIMEOUT 		30

#define TAG_WIFI 						"WIFI"


esp_err_t wifi_event_handler(void *ctx, system_event_t *event);
void disconnection_handler(int reason);
void sc_callback(smartconfig_status_t status, void *pdata);
void smartconfig_task(void *void_parameters);
void wifi_cfg(void);


#endif /* WIFI_H_ */
