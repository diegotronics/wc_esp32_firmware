#include "wc_esp32_mqtt.h"

esp_mqtt_client_handle_t client = NULL;

DRAM_ATTR char data_to_camera[32];
DRAM_ATTR char *camera_id;

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_CONNECTED");

            if (strcmp(camera_id, NVS_READ_ERROR) == 0){
            	spi_to_camera(PROTOCOL_ASK_ID);
            }else{
            	mqtt_pub_id(camera_id);
            	mqtt_sub_id(camera_id);
            }

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DATA");
            ESP_LOGI(TAG_MQTT, "TOPIC=%.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG_MQTT, "DATA=%.*s", event->data_len, event->data);


            sprintf(data_to_camera,"%.*s", event->data_len, event->data);
            spi_to_camera(data_to_camera);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}


void mqtt_pub_id(char *data){
	esp_mqtt_client_publish(client, MQTT_TOPIC_PUB_ID, data, 24, MQTT_QOS, false);
}

void mqtt_sub_id(char *cam_id){
	int msg_id;
	char topic_id[34];
	sprintf(topic_id, "%s%s", MQTT_TOPIC_SUB, cam_id);
	msg_id = esp_mqtt_client_subscribe(client, topic_id, 0);
	ESP_LOGI(TAG_MQTT, "sent subscribe successful, msg_id = %d, topic = %s", msg_id, topic_id);

	nvs_set_str(my_handle, "camera_id", (char *) cam_id);
	nvs_commit(my_handle);
}

void mqtt_cfg(void)
{
	while(false == connected){
		vTaskDelay(2000 / portTICK_RATE_MS);
	}

	camera_id = nvs_read("camera_id");

	const esp_mqtt_client_config_t mqtt_cfg = {
        .host = SERVER_MQTT_ADDRESS,
		.port = SERVER_MQTT_PORT,
        .event_handle = mqtt_event_handler,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}
