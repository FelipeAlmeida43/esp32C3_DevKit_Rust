/*
 * Timer.c
 *
 *  Created on: 23 de fev. de 2024
 *      Author: Felipe
 */
#include <stdint.h>
#include <stdbool.h>
#include <esp_timer.h>
#include <esp_task_wdt.h>
#include "rtc_wdt.h"
#include <esp_log.h>
uint32_t counterModoAP =0,
		minutes_counter =0,
		millis_led	=0;

bool flagResetModoAP = false;

bool client_ap_connected,ap_mode,flagResetModem;

#define TIMEOUT_MODO_AP 2
#define DAY_COUNTER	500
#define TIMER_INTERVAL_SEC  (1) // One hour interval

static void timer_callback(void *arg)
{

	//rtc_wdt_disable();
    static int hours_counter = 0;
    if(ap_mode && (!client_ap_connected)){
    	counterModoAP++;
    	if(counterModoAP > TIMEOUT_MODO_AP ){

    		counterModoAP =0;
    		flagResetModoAP = true;
    	}
    }
    hours_counter++;
    minutes_counter = hours_counter;
    //ESP_LOGI(__func__,"Timer triggered! Minutes count: %d\r\n", hours_counter);

    if (hours_counter >= DAY_COUNTER) {


        flagResetModem = true;
        ESP_LOGI(__func__,"24 hours reached. Resetting GPIO.\r\n");
        hours_counter = 0; // Reset the counter
    }
   // rtc_wdt_enable();
}
void initializeTimer1(){
	esp_timer_create_args_t timer_config = {
		        .callback = timer_callback,
		        .arg = NULL,
		        .dispatch_method = ESP_TIMER_TASK,
		        .name = "my_timer"
		    };
		    esp_timer_handle_t timer;
		    esp_timer_create(&timer_config, &timer);

		    // Start the timer
	  esp_timer_start_periodic(timer, (TIMER_INTERVAL_SEC * 1000)); // Convert to microseconds
}

