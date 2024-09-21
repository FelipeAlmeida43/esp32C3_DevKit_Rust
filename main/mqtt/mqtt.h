#ifndef MQTT_H
#define MQTT_H

#include "string.h"
#include <stdint.h>
#include "mqtt_client.h"

#define MQTT_SERVER "mqtts://88dd94022fb34d488b070b206c8bfc98.s1.eu.hivemq.cloud:8883"
//const char* MQTT_SERVER = "mqtt3.thingspeak.com";
#define MQTT_PORT 8883
/*
#define MQTT_USER "hivemq.webclient.1724432358116"
#define MQTT_PASS "n!1I7<DfCjl$PU0Gcb6>"
*/
#define MQTT_USER "esp32c3"
#define MQTT_PASS "esp32_C3"
#define QOS	2

//char MAC_Address[18];


void mqtt_app_start(void);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
		int32_t event_id, void *event_data);
		
#endif  