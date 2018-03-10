#ifndef __I2C_WRAPPER_H
#define __I2C_WRAPPER_H

#include<stdint.h>
#include<pthread.h>

pthread_mutex_t bus_mutex;

pthread_mutex_t bus;

extern inline int i2c_bus_init(int bus_number);
extern inline int i2c_bus_access(int i2c_bus_desc, uint8_t slave_address);
extern inline void i2c_bus_free();
extern size_t i2c_bus_write(int i2c_bus_desc, uint8_t register_address, char * buffer, size_t length); 
extern size_t i2c_bus_read(int i2c_bus_desc, uint8_t register_address, char * buffer, size_t length);


#endif
