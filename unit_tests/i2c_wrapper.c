#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

#include<linux/i2c-dev.h>
#include<linux/i2c.h>

#include<sys/ioctl.h>

#include<stdio.h>
#include<stdint.h>
#include<pthread.h>

#include "i2c_wrapper.h"
#include <strings.h>
#include <string.h>

extern pthread_mutex_t bus_mutex;

int i2c_bus_init(int bus_number)
{
	int i2c_bus_desc;
	char filename[20];


        sprintf(filename, "/dev/i2c-%d", bus_number);
        i2c_bus_desc = open(filename, O_RDWR);
	pthread_mutex_init(&bus_mutex, NULL);

	return i2c_bus_desc;
}

int i2c_bus_access(int i2c_bus_desc, uint8_t slave_address)
{
	pthread_mutex_lock(&bus_mutex);
        return ioctl(i2c_bus_desc, I2C_SLAVE, slave_address);
}

void i2c_bus_free()
{
	pthread_mutex_unlock(&bus_mutex);
}

size_t i2c_bus_write(int i2c_bus_desc, uint8_t register_address,  uint8_t *buffer, size_t length)
{
	uint8_t buf[length+1];
	int retval;

	bzero(buf, sizeof(buf));

	buf[0] = register_address;

	memcpy(buf+1, buffer, length);

	//sprintf(buf+1, "%s", buffer);

	retval = write(i2c_bus_desc, buf, length+1);
	return --retval;
}

size_t i2c_bus_read(int i2c_bus_desc, uint8_t register_address, uint8_t* buffer, size_t length)
{
	uint8_t buf[length];
	int retval;

	bzero(buf, sizeof(buf));

	buf[0] = register_address;
	retval = write(i2c_bus_desc, buf, 1);

	if(retval < 0)
	{
		perror("Error while reading contents");
	}

	retval = read(i2c_bus_desc, buf, length);

	memcpy(buffer, buf, length);
	return retval;
}
