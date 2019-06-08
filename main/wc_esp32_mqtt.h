/*
 * wc_esp32_mqtt.h
 *
 *  Created on: 19 nov. 2018
 *      Author: -
 */

#ifndef MAIN_WC_ESP32_MQTT_H_
#define MAIN_WC_ESP32_MQTT_H_

#include "esp_log.h"
#include "mqtt_client.h"
#include "wc_esp32_spi.h"
#include "wc_esp32_udp.h"


#define TAG_MQTT	"MQTT"

#define SERVER_MQTT_ADDRESS		SERVER_ADDRESS
#define SERVER_MQTT_PORT		1883

#define MQTT_QOS				0

#define MQTT_TOPIC_PUB_ID	"AI/ID"
#define MQTT_TOPIC_SUB		"AI/CAMERA/"
#define MQTT_TOPIC_PUB		"AI/CAMERA/"


#define PROTOCOL_ASK_ID "AI.ID.00.3C"

void mqtt_cfg(void);
void mqtt_sub_id(char *camera_id);
void mqtt_pub_id(char *data);

#endif /* MAIN_WC_ESP32_MQTT_H_ */
