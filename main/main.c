#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "ws2812/ws2812.h"
#include "timer/Timer.h"
#include "button/button.h"
#include "sensor/i2Csensor.h"
#include "mqtt/mqtt.h"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
extern bool flagResetModem;
extern esp_mqtt_client_handle_t mqtt_client;
#define GPIO_PIN 2
#define WIFI_SSID "WIFI_MESH_IST"
#define WIFI_PASS "ac1ce0ss6_mesh"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1



static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

static const char *TAG = "workshop";

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 30) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGW(TAG, "Trying to connect to WiFi");
			ws2812_set_color(32, 0, 0);
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            ws2812_set_color(0, 0, 0);
        }
        ESP_LOGE(TAG, "Failed to connect to WiFi");
        ws2812_set_color(0, 0, 32);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        ws2812_set_color(0, 32, 0);
        s_retry_num = 0;
        mqtt_app_start();
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &event_handler,
            NULL,
            &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &event_handler,
            NULL,
            &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            .sae_h2e_identifier = "",
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected!");
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "Failed to connect!");
    }
}
float shtc3_raw_to_temperature(uint16_t raw_value) {
    return -45 + 175 * (raw_value / 65535.0);
}

float shtc3_raw_to_humidity(uint16_t raw_value) {
    return 100 * (raw_value / 65535.0);
}
void app_main(void)
{
	ws2812_init(GPIO_PIN);
	ws2812_set_color(32, 0, 0);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

   wifi_init_sta();
    button_init();
    i2c_master_init();
    shtc3_data_t sensor;
    uint16_t sensor_id;
    vTaskDelay(pdMS_TO_TICKS(2000));  // Main loop delay
    shtc3_send_command(SHTC3_WAKEUP_CMD);
        	       vTaskDelay(pdMS_TO_TICKS(1)); // Wait for wake up
    if (shtc3_read_id(&sensor_id) == ESP_OK) {
            printf("Sensor ID: 0x%04X\n", sensor_id);
        } else {
            printf("Failed to read sensor ID\n");
        }
    while(1){

    	int button_state = read_button_debounced();

		if(1){
    	//if (button_state != -1) {
    		if(1){
    		//if (button_state == 0) {

    	            	//ws2812_set_color(32, 0, 0);
    	            	ESP_LOGI(__func__,"Button Pressed\r\n");
    	            	shtc3_send_command(SHTC3_MEASURE_CMD);
    	            	vTaskDelay(pdMS_TO_TICKS(15)); // Measurement time
    	            	if (shtc3_read_data(&sensor) == ESP_OK) {

    	            		float temperature = shtc3_raw_to_temperature(sensor.temperature);
    	            	    float humidity = shtc3_raw_to_humidity(sensor.humidity);
    	            	    printf("Temperature: %.2f C, Humidity: %.2f%%\n", temperature, humidity);
    	            	    char json[100];
    	            	    sprintf(json,"{Temperature: %.2f , Humidity: %.2f}", temperature, humidity);
    	            	    esp_mqtt_client_publish(mqtt_client, "esp32c3/Weather", json, strlen(json), QOS, 0);
    	            	  
    	            	}else {
     	                          printf("Failed to read from sensor\n");
     	                      }
    	            }else{
    	                //printf("Button released!\n");
    	            }
    	        } else {
    	            //printf("Button state unstable, skipping\n");
    	        }

    	        vTaskDelay(pdMS_TO_TICKS(1000));  // Main loop delay
    }
}
