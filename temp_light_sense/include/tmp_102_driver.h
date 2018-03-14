#ifndef __TMP_102_DRIVER_H
#define __TMP_102_DRIVER_H

#define SLAVE_ADDRESS_TMP 0x48 

#define TEMPERATURE_REGISTER 	0x00
#define CONFIG_REGISTER 	0x01
#define TLOW_REGISTER	0x02
#define THIGH_REGISTER	0x03

#define SHUTDOWN_MODE 0x01<<0
#define INTERRUPT_MODE 0x01<<1
#define EXTENDED_MODE 0x01<<4
#define ACTIVE_HIGH_ALERT_PIN 0x01<<2

#define FAULT_QUEUE_COUNT 0x01<<3
#define CONVERSION_RATE 0x03<<6

#define RESOLUTION 0.0625

#define GPIO_INTERRUPT_PIN 65

int tmp_102_init(int i2c_bus_desc);
int tmp_102_read_temperature_reg(int i2c_bus_desc, float *temperature);
int tmp_102_read_config_reg(int i2c_bus_desc, uint16_t *config_reg);
int tmp_102_write_config_reg(int i2c_bus_esc, uint16_t config_reg);

int tmp_102_read_TLow_reg(int i2c_bus_desc, float *TLow_reg);
int tmp_102_read_THigh_reg(int i2c_bus_desc, float *THigh_reg);
int tmp_102_write_TLow_reg(int i2c_bus_desc, float TLow_register);
int tmp_102_write_THigh_reg(int i2c_bus_esc, float THigh_reg);



#endif
