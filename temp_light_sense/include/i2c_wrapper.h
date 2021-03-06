#ifndef __I2C_WRAPPER_H
#define __I2C_WRAPPER_H

#include<stdint.h>
#include<pthread.h>

pthread_mutex_t bus_mutex;

pthread_mutex_t bus;

int i2c_bus_init(int bus_number);
int i2c_bus_access(int i2c_bus_desc, uint8_t slave_address);
void i2c_bus_free();
size_t i2c_bus_write(int i2c_bus_desc, uint8_t register_address, uint8_t* buffer, size_t length); 
size_t i2c_bus_read(int i2c_bus_desc, uint8_t register_address, uint8_t* buffer, size_t length);


#endif
