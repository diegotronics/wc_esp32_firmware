/*
 * wc_esp32_nvs.c
 *
 *  Created on: 20 sept. 2018
 *      Author: Diego Perez
 *      email: diegoperez1295@gmail.com
 */
#include "wc_esp32_nvs.h"

/* NVS CONFIGURATION */
void nvs_cfg(void){
    esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
    }
	ESP_ERROR_CHECK(ret);
	ESP_LOGI(TAG_NVS, "NVS ready");
	ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
}

/* READ A VALUE OF NVS */
char * nvs_read(char* key){
	size_t required_size;
	nvs_get_str(my_handle, key, NULL, &required_size);
	char* value = malloc(required_size);
	esp_err_t err = nvs_get_str(my_handle, key, value, &required_size);

	switch (err) {
		case ESP_OK:
			ESP_LOGI(TAG_NVS,"key: %s Value: %s", key, value);
			return value;
			break;
		case ESP_ERR_NVS_NOT_FOUND:
			ESP_LOGI(TAG_NVS,"The value is not initialized yet!\n");
			return NVS_READ_ERROR;
			break;
		default :
			ESP_LOGI(TAG_NVS,"Error (%s) reading!\n", esp_err_to_name(err));
			return NVS_READ_ERROR;
	}
}
void nvs_clear_credentials(void){
	nvs_erase_key(my_handle, "ssid");
	nvs_erase_key(my_handle, "password");
	ESP_LOGI(TAG_NVS,"Credentials cleared\n");
	nvs_commit(my_handle);
}
