/*
 * i2Csensor.h
 *
 *  Created on: 21 de jul. de 2024
 *      Author: FelipeAlmeida
 */

#ifndef MAIN_SENSOR_I2CSENSOR_H_
#define MAIN_SENSOR_I2CSENSOR_H_

#define I2C_MASTER_SCL_IO 8     // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO 10      // GPIO number for I2C master data
#define I2C_MASTER_NUM I2C_NUM_0 // I2C port number for master
#define I2C_MASTER_FREQ_HZ 100000 // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE 0  // I2C master doesn't need buffer
#define I2C_MASTER_RX_BUF_DISABLE 0  // I2C master doesn't need buffer

#define SHTC3_SENSOR_ADDR 0x70    // I2C address of the SHTC3 sensor

#define SHTC3_WAKEUP_CMD 0x3517
#define SHTC3_SLEEP_CMD 0xB098
#define SHTC3_MEASURE_CMD 0x7CA2
#define SHTC3_SOFT_RESET_CMD 0x805D
#define SHTC3_ID_CMD 0xEFC8
#define MEAS_T_RH_POLLING  0x7866 // meas. read T first, clock stretching disabled
#define MEAS_T_RH_CLOCKSTR  0x7CA2 // meas. read T first, clock stretching enabled
#define  MEAS_RH_T_POLLING  0x58E0 // meas. read RH first, clock stretching disabled
#define MEAS_RH_T_CLOCKSTR 0x5C24  // meas. read RH first, clock stretching enabled


typedef struct {
    uint16_t temperature;
    uint16_t humidity;
} shtc3_data_t;
void i2c_master_init();
esp_err_t shtc3_send_command(uint16_t command);
esp_err_t shtc3_read_data(shtc3_data_t *data);
esp_err_t shtc3_read_id(uint16_t *id);
#endif /* MAIN_SENSOR_I2CSENSOR_H_ */
