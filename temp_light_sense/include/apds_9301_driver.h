#ifndef __APDS_9301_DRIVER_H
#define __APDS_9301_DRIVER_H

#include <stdint.h>

#define SLAVE_ADDRESS_FLOAT 0x39
#define SLAVE_ADDRESS_HIGH 0x49
#define SLAVE_ADDRESS_LOW 0x29

#ifndef SLAVE_ADDRESS
#define SLAVE_ADDRESS SLAVE_ADDRESS_FLOAT
#endif

#define CONTROL_REGISTER 0x00
#define TIMING_REGISTER 0x01
#define THRESHLOWLOW_REGISTER 0x02
#define THRESHLOWHIGH_REGISTER 0x03
#define THRESHHIGHLOW_REGISTER 0x04
#define THRESHHIGHHIGH_REGISTER 0x05
#define INTERRUPT_REGISTER 0x06
#define DATA0LOW_REGISTER 0x0C
#define DATA0HIGH_REGISTER 0x0D
#define DATA1LOW_REGISTER 0x0E
#define DATA1HIGH_REGISTER 0x0F
#define ID_REGISTER 0x0A

#define COMMAND_BYTE 0x80
#define WORD_MODE 0x20
#define INTERRUPT_CLEAR 0x40

#define POWER_UP 0x03
#define POWER_DOWN 0x00
#define INTEGRATION_TIME 0x02
#define GAIN_MODE 0x00

#define ADC_CHANNEL_0 0
#define ADC_CHANNEL_1 1

int apds_9301_init(int i2c_bus_desc);
int apds_9301_shutdown(int i2c_bus_desc);
int16_t apds_9301_read_control_reg(int i2c_bus_desc);
int apds_9301_write_control_reg(int i2c_bus_desc, uint8_t control_reg_data);
int16_t apds_9301_read_timing_reg(int i2c_bus_desc);
int apds_9301_write_timing_reg(int i2c_bus_desc, uint8_t timing_reg_data);
int16_t apds_9301_read_interrupt_reg(int i2c_bus_desc);
int16_t apds_9301_read_id_reg(int i2c_bus_desc);
int apds_9301_write_interrupt_reg(int i2c_bus_desc, uint8_t interrupt_reg_data);
int32_t apds_9301_read_thresh_low_reg(int i2c_bus_desc);
int apds_9301_write_thresh_low_reg(int i2c_bus_desc, uint16_t thresh_low_reg);
int32_t apds_9301_read_thresh_high_reg(int i2c_bus_desc);
int apds_9301_write_thresh_high_reg(int i2c_bus_desc, uint16_t thresh_high_reg);
int32_t apds_9301_read_adcn(int i2c_bus_desc, int adc_channel_number);


#endif
