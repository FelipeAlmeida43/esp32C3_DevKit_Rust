
#include "mqtt_client.h"
#include "esp_mac.h"
#include "mqtt.h"
#include "cJSON.h"
#include "esp_log.h"
#include <stdint.h>
#include <stdio.h>
esp_mqtt_client_handle_t mqtt_client;
bool b_mqttStarted = false;
bool mqtt_connected;
/*
extern uint16_t addressRegister ;
extern uint16_t countRegister ;
extern int functionCode,mbSlaveAddres,ledState;
*/
extern uint32_t millis_led;
//extern const char* MQTT_SERVER;
/****** root certificate *********/

static const char root_ca[]  = 
"-----BEGIN CERTIFICATE-----\n"
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
"-----END CERTIFICATE-----\n";

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
		int32_t event_id, void *event_data) {
	//ESP_LOGD(TAG1, "Event dispatched from event loop base=%s, event_id=%d",
		//	base, event_id);
	esp_mqtt_event_handle_t event = event_data;

	//esp_mqtt_client_handle_t client = event->client;
	mqtt_client = event->client;
	//int msg_id=0;
	switch ((esp_mqtt_event_id_t) event_id) {
	case MQTT_EVENT_CONNECTED:
		printf("MQTT_EVENT_CONNECTED");
		esp_mqtt_client_subscribe(mqtt_client, "edc/modbus/config", 2);
		mqtt_connected = true;
		/*
		ping_sucess_flag = true;
		mqtt_connected = true;
		sprintf(topic_formatted, "edc/%s/relay", &id);
		esp_mqtt_client_subscribe_single(mqtt_client, (char*) &topic_formatted,
				QOS);

		sprintf(topic_time, "edc/%s/time", &id);
		esp_mqtt_client_subscribe_single(mqtt_client, (char*) &topic_time, QOS);

		sprintf(topic_update, "edc/%s/update", &id);
		esp_mqtt_client_subscribe_single(mqtt_client, (char*) topic_update,
				QOS);
		flagSavedFile = checkSavedFile("/json.txt");
		char str[20];
		sprintf(str, "RSSI : %d", acess_point.rssi);
		sprintf(topic_signal, "edc/%s/signal", &id);
		esp_mqtt_client_publish(mqtt_client, topic_signal, (char*) &str,
				strlen((char*) &str), QOS, 0);
*/
		break;
	case MQTT_EVENT_DISCONNECTED:
		//printf( "MQTT_EVENT_DISCONNECTED");
		printf("MQTT_EVENT_DISCONNECTED\r\n");
		//esp_mqtt_client_reconnect(mqtt_client);
		/* //uart_write_bytes(SERIAL, "MQTT_EVENT_DISCONNECTED\r\n", strlen("MQTT_EVENT_DISCONNECTED\r\n"));
		//
		millis_led = 2000;
		ping_sucess_flag = false;
		msgtest = false;
		mqttReconect++;
		if (mqttReconect > MQTT_CONNECTION_RETRY) {
			esp_mqtt_client_reconnect(mqtt_client);
			printf("MQTT_EVENT_RECONECT");
			//uart_write_bytes(SERIAL, "MQTT_RECONECT\r\n", strlen("MQTT_RECONECT\r\n"));
			mqttReconect = 0;
		}
		mqtt_connected = false; */
		mqtt_connected = false;
		break;

	case MQTT_EVENT_SUBSCRIBED:
		//uart_write_bytes(SERIAL, "MQTT_EVENT_SUBSCRIBED\r\n", strlen("MQTT_EVENT_SUBSCRIBED\r\n"));
		printf("MQTT_EVENT_SUBSCRIBED, msg_id=%d\r\n", event->msg_id);

		break;
	case MQTT_EVENT_UNSUBSCRIBED:

		printf("MQTT_EVENT_UNSUBSCRIBED, msg_id=%d\r\n", event->msg_id);
		//uart_write_bytes(SERIAL, "MQTT_EVENT_UNSUBSCRIBED\r\n", strlen("MQTT_EVENT_UNSUBSCRIBED\r\n"));
		break;
	case MQTT_EVENT_PUBLISHED:
		
		printf("MQTT_EVENT_PUBLISHED, msg_id=%d\r\n", event->msg_id);
		//uart_write_bytes(SERIAL, "MQTT_EVENT_PUBLISHED\r\n", strlen("MQTT_EVENT_PUBLISHED\r\n"));
		break;
	case MQTT_EVENT_DATA:
		printf("DATA =%.*s\r\n", event->data_len, event->data);
		cJSON *root = cJSON_Parse(event->data);
    if (root == NULL) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }

    // Get the first object in the array
    cJSON *item = cJSON_GetArrayItem(root, 0);
    if (item == NULL) {
        printf("Failed to get JSON array item\n");
        cJSON_Delete(root);
        return;
    }
	

    // Clean up
    cJSON_Delete(root);

		break;
	case MQTT_EVENT_ERROR:
		printf("MQTT_EVENT_ERROR\r\n");
		//uart_write_bytes(SERIAL, "MQTT_EVENT_ERROR\r\n", strlen("MQTT_EVENT_ERROR\r\n"));
		
		if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
			printf("Last error code reported from esp-tls: 0x%x",
					event->error_handle->esp_tls_last_esp_err);
			printf("Last tls stack error number: 0x%x",
					event->error_handle->esp_tls_stack_err);
			printf("Last captured errno : %d (%s)",
					event->error_handle->esp_transport_sock_errno,
					strerror(event->error_handle->esp_transport_sock_errno));
		} else if (event->error_handle->error_type
				== MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
			printf("Connection refused error: 0x%x",
					event->error_handle->connect_return_code);
		} else {
			
		}
		
		break;
	case MQTT_EVENT_BEFORE_CONNECT:
		printf("MQTT_EVENT_BEFORE_CONNECT\r\n");
		
		break;
	case MQTT_EVENT_DELETED:
		printf("MQTT_EVENT_DELETED\r\n");
		
		break;
	default:
		printf("Other event id:%d", event->event_id);
		break;
	}
}
void mqtt_app_start(void) {

	const esp_mqtt_client_config_t mqtt_cfg = {
	//.broker.address.uri = "mqtt3.thingspeak.com",
	//.broker.address.hostname =  (const char*)MQTT_SERVER,
	.broker.address.uri =  (const char*)MQTT_SERVER, 
	.broker.address.port =	MQTT_PORT, 
	.credentials.client_id = (const char*)MQTT_USER,
	.credentials.username = (const char*)MQTT_USER,
	.credentials.authentication.password = (const char*) MQTT_PASS,
	.broker.verification.certificate = root_ca, // broker certificate
	//.credentials.authentication.certificate = root_ca, // Client certificate
	.buffer.size = 4096,
	.session.disable_clean_session = false,
	.network.disable_auto_reconnect = false,
	//.broker.address.transport =	MQTT_TRANSPORT_OVER_SSL, 
	.session.protocol_ver =	MQTT_PROTOCOL_V_5,
	.session.keepalive = 60,
	//.network.timeout_ms =000,
	//.network.refresh_connection_after_ms =10000,
	
	
	
	};
	//printf("[APP] Free memory: %u bytes",(uint8_t *) esp_get_free_heap_size());
	mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	/* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
	esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY,
			mqtt_event_handler, NULL);

	printf("MQTT ID %s\r\n", mqtt_cfg.credentials.client_id);
	if (esp_mqtt_client_start(mqtt_client) == ESP_OK){
		b_mqttStarted = true;
		printf("MQTT STARTED \r\n");
		//uart_write_bytes(SERIAL, "MQTT_STARTED\r\n", strlen("MQTT_STARTED\r\n"));
		
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	} else {
		b_mqttStarted = false;
		printf("MQTT ERROR\r\n");
		//uart_write_bytes(SERIAL, "MQTT_ERROR\r\n", strlen("MQTT_ERROR\r\n"));
		//esp_mqtt_client_reconnect(mqtt_client);
	}

}