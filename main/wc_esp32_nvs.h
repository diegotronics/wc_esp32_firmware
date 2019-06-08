/*
 * wc_esp32_nvs.h
 *
 *  Created on: 20 sept. 2018
 *      Author: Diego Perez
 *      email: diegoperez1295@gmail.com
 */

#ifndef MAIN_WC_ESP32_NVS_H_
#define MAIN_WC_ESP32_NVS_H_

/* INCLUDES */
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_err.h"

/* DEFINES */
#define TAG_NVS "NVS"
#define NVS_READ_ERROR "0"

/* GLOBAL VARIABLES */
nvs_handle my_handle;

/* FUNCTIONS */
void nvs_cfg(void);
char * nvs_read(char* key);
void nvs_clear_credentials(void);

#endif /* MAIN_WC_ESP32_NVS_H_ */
