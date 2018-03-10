#ifndef __APDS_9301_DRIVER_H
#define __APDS_9301_DRIVER_H

#define SLAVE_ADDRESS_FLOAT 0x39
#define SLAVE_ADDRESS_HIGH 0x49
#define SLAVE_ADDRESS_LOW 0x29

#ifndef __SLAVE_ADDRESS_SELECT
#define __SLAVE_ADDRESS_SELECT
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

#define COMMAND_BYTE 0x80
#define WORD_MODE 0x20
#define INTERRUPT_CLEAR 0x40

#define POWER_UP 0x03
#define INTEGRATION_TIME 0x01
#define GAIN_MODE 0x10

int apds_9301_init(int i2c_bus_desc);





#endif
