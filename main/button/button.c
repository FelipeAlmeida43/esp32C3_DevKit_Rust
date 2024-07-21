/*
 * button.c
 *
 *  Created on: 21 de jul. de 2024
 *      Author: FelipeAlmeida
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "button.h"
void button_init() {
    gpio_config_t io_conf;
    // Configure GPIO as input
    io_conf.intr_type = GPIO_INTR_DISABLE;  // No interrupt
    io_conf.mode = GPIO_MODE_INPUT;         // Input mode
    io_conf.pin_bit_mask = (1ULL << BUTTON_GPIO);  // Bit mask for GPIO 9
    io_conf.pull_down_en = 0;               // Disable pull-down
    io_conf.pull_up_en = 1;                 // Enable pull-up

    gpio_config(&io_conf);
}

int read_button_debounced() {
    int current_state = gpio_get_level(BUTTON_GPIO);
    vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY_MS));
    int new_state = gpio_get_level(BUTTON_GPIO);

    if (current_state == new_state) {
        return current_state;
    } else {
        return -1; // Indicate unstable state
    }
}



