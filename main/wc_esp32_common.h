/*
 * wc_esp32_common.h
 *
 *  Created on: 11 dic. 2018
 *      Author: -
 */

#ifndef MAIN_WC_ESP32_COMMON_H_
#define MAIN_WC_ESP32_COMMON_H_

/* INCLUDES */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/igmp.h"
#include "lwip/sys.h"
#include "lwip/err.h"

#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_intr_alloc.h"
#include "esp_smartconfig.h"

#include "nvs_flash.h"

#include "soc/rtc_cntl_reg.h"
#include "rom/cache.h"

#include "driver/spi_slave.h"
#include "driver/gpio.h"

#include "esp_wifi.h"

extern bool connected;



#endif /* MAIN_WC_ESP32_COMMON_H_ */
