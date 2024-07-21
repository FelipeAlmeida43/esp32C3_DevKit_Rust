/*
 * i2Csensor.c
 *
 *  Created on: 21 de jul. de 2024
 *      Author: FelipeAlmeida
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "i2Csensor.h"
#include "driver/i2c_master.h"


void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_10,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = GPIO_NUM_8,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,

    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
   esp_err_t ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
   if(ret== ESP_OK){
	   ESP_LOGI(__func__,"I2C Initialized!\r\n");
   }else ESP_LOGE(__func__,"I2C ERROR!\r\n");
}
uint8_t calculate_crc(uint8_t *data, uint8_t len) {
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

esp_err_t shtc3_send_command(uint16_t command) {
    uint8_t cmd_buffer[2] = {command >> 8, command & 0xFF};
    return i2c_master_write_to_device(I2C_MASTER_NUM, SHTC3_SENSOR_ADDR, cmd_buffer, sizeof(cmd_buffer), 1000 / portTICK_PERIOD_MS);
}
esp_err_t shtc3_read_data(shtc3_data_t *data) {
    uint8_t read_buffer[6];
    esp_err_t ret = i2c_master_read_from_device(I2C_MASTER_NUM, SHTC3_SENSOR_ADDR, read_buffer, sizeof(read_buffer), 1000 / portTICK_PERIOD_MS);
    if (ret != ESP_OK) {
        return ret;
    }
    // Check CRC for temperature
        uint8_t crc = calculate_crc(read_buffer, 2);
        if (crc != read_buffer[2]) {
            return ESP_ERR_INVALID_CRC;
        }

        // Check CRC for humidity
        crc = calculate_crc(read_buffer + 3, 2);
        if (crc != read_buffer[5]) {
            return ESP_ERR_INVALID_CRC;
        }
    data->temperature = (read_buffer[0] << 8) | read_buffer[1];
    data->humidity = (read_buffer[3] << 8) | read_buffer[4];

    return ESP_OK;
}
esp_err_t shtc3_read_id(uint16_t *id) {
    uint8_t cmd_buffer[2] = {SHTC3_ID_CMD >> 8, SHTC3_ID_CMD & 0xFF};
    //uint8_t cmd_buffer = SHTC3_ID_CMD;
    uint8_t read_buffer[3];

    esp_err_t ret = i2c_master_write_to_device(I2C_MASTER_NUM, SHTC3_SENSOR_ADDR,cmd_buffer, sizeof(cmd_buffer), 1000/ portTICK_PERIOD_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(__func__, "Failed to send ID command: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = i2c_master_read_from_device(I2C_MASTER_NUM, SHTC3_SENSOR_ADDR, read_buffer, sizeof(read_buffer), 1000/ portTICK_PERIOD_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(__func__, "Failed to read ID: %s", esp_err_to_name(ret));
        return ret;
    }

    uint8_t crc = calculate_crc(read_buffer, 2);
    if (crc != read_buffer[2]) {
        ESP_LOGE(__func__, "CRC check failed for ID");
        return ESP_ERR_INVALID_CRC;
    }

    *id = (read_buffer[0] << 8) | read_buffer[1];
    return ESP_OK;
}





