#include "i2c_wrapper.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

#include<linux/i2c-dev.h>
#include<linux/i2c.h>

#include<sys/ioctl.h>

#include<stdio.h>
#include<string.h>

inline int i2c_bus_init(int bus_number)
{
	int i2c_bus_desc;
	char filename[20];

        sprintf(filename, "/dev/i2c-%d", bus_number);
        i2c_bus_desc = open(filename, O_RDWR);
	pthread_mutex_init(&bus_mutex, NULL);
	
	return i2c_bus_desc;
}

inline int i2c_bus_access(int i2c_bus_desc, uint8_t slave_address)
{
	pthread_mutex_lock(&bus_mutex);
        return ioctl(i2c_bus_desc, I2C_SLAVE, slave_address);
}

inline void i2c_bus_free()
{
	pthread_mutex_unlock(&bus_mutex);
}

size_t i2c_bus_write(int i2c_bus_desc, uint8_t register_address, char * buffer, size_t length)
{
	char buf[length+1];
	int retval;

	sprintf(buf, "%c%s", (char )register_address, buffer);

	retval = write(i2c_bus_desc, buf, length+1);
	return --retval;
}

size_t i2c_bus_read(int i2c_bus_desc, uint8_t register_address, char * buffer, size_t length)
{
	char buf[length];
	int retval;

	bzero(buf, sizeof(buf));

	buf[0] = (char )register_address;
	retval = read(i2c_bus_desc, buf, length);

	memcpy(buffer, buf, length);
	return retval;
}


