/*
 * wifi.c
 *
 *  Created on: 14 ago. 2018
 *      Author: -
 */
#include "wc_esp32_wifi.h"

/* FreeRTOS event group to signal when we are connected & ready to make a request */
EventGroupHandle_t wifi_event_group;

TaskHandle_t smartconfig_task_handler;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;



esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
		case SYSTEM_EVENT_STA_START:
			esp_wifi_connect();
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
			ESP_LOGI(TAG_WIFI, "got ip:%s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			connected = true;
			xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			connected = false;
			xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);

			ESP_LOGI(TAG_WIFI,"Disconnected from %.*s, reason: %d",
					(int) event->event_info.disconnected.ssid_len,
					event->event_info.disconnected.ssid,
					event->event_info.disconnected.reason);

			disconnection_handler(event->event_info.disconnected.reason);
			break;
		default:
			break;
    }

    return ESP_OK;
}
void disconnection_handler(int reason){
	switch(reason){
		case WIFI_REASON_AUTH_EXPIRE:
			//nvs_clear_credentials();
			esp_smartconfig_stop();
			vTaskDelete(smartconfig_task_handler);
			xTaskCreate(&smartconfig_task, "smartconfig_task", 2048, NULL, 5, &smartconfig_task_handler);
			break;
		case WIFI_REASON_NOT_ASSOCED:
			//nvs_clear_credentials();
			xTaskCreate(&smartconfig_task, "smartconfig_task", 2048, NULL, 5, &smartconfig_task_handler);
			break;
		default:
			esp_wifi_connect();
			break;
	}
}

void sc_callback(smartconfig_status_t status, void *pdata)
{
    switch (status) {
        case SC_STATUS_WAIT:
            ESP_LOGI(TAG_WIFI, "SC_STATUS_WAIT");
            break;
        case SC_STATUS_FIND_CHANNEL:
            ESP_LOGI(TAG_WIFI, "SC_STATUS_FINDING_CHANNEL");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            ESP_LOGI(TAG_WIFI, "SC_STATUS_GETTING_SSID_PSWD");
            break;
        case SC_STATUS_LINK:
            ESP_LOGI(TAG_WIFI, "SC_STATUS_LINK");
            wifi_config_t *wifi_config = pdata;
            ESP_LOGI(TAG_WIFI, "SSID:%s", wifi_config->sta.ssid);
            ESP_LOGI(TAG_WIFI, "PASSWORD:%s", wifi_config->sta.password);
            //Save in NVS
            nvs_set_str(my_handle, "ssid", (char *) wifi_config->sta.ssid);
            nvs_set_str(my_handle, "password", (char *) wifi_config->sta.password);
            nvs_commit(my_handle);

            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;
        case SC_STATUS_LINK_OVER:
            ESP_LOGI(TAG_WIFI, "SC_STATUS_LINK_OVER");
            xEventGroupSetBits(wifi_event_group, ESPTOUCH_DONE_BIT);
            break;
        default:
            break;
    }
}


void smartconfig_task(void *void_parameters)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    ESP_ERROR_CHECK( esp_smartconfig_start(sc_callback) );
    uxBits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
    while (1) {
        uxBits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if(uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG_WIFI, "WiFi Connected to ap");
        }
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG_WIFI, "Smart_config over");
            esp_smartconfig_stop();
            vTaskDelete(smartconfig_task_handler);
        }
    }
}

void wifi_cfg(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(wifi_event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    char * ssid = nvs_read("ssid");
    char * password = nvs_read("password");
	wifi_config_t wifi_config;
	memset(&wifi_config, 0, sizeof(wifi_config));
    if (strcmp(ssid, NVS_READ_ERROR) == 0 || strcmp(password, NVS_READ_ERROR) == 0){
       	xTaskCreate(&smartconfig_task, "smartconfig_task", 2048, NULL, 5, &smartconfig_task_handler);
    }
    else{
    	wifi_sta_config_t *stacfg = &wifi_config.sta;
    	strncpy((char *) stacfg->ssid, ssid, sizeof(stacfg->ssid));
    	strncpy((char *) stacfg->password, password, sizeof(stacfg->password));
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
