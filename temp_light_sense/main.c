#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<err.h>
#include<errno.h>

#include<string.h>

#include<linux/i2c-dev.h>
#include<linux/i2c.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

#include<sys/ioctl.h>

#include "include/i2c_wrapper.h"
#include "include/apds_9301_driver.h"

int bus_number = 2;

int main()
{
	uint16_t word, adc0, adc1;
	char filename[20];
	int i2c_bus_desc;
	int retval, regaddr;
	char buf[10];

	regaddr = 0x52;

	i2c_bus_desc = i2c_bus_init(bus_number);

	if(i2c_bus_desc < 0)
	{
		printf("Error opening a i2c character device file: %s", strerror(errno));
		return 1;
	}

	apds_9301_init(i2c_bus_desc);
	
	adc0 = apds_9301_read_adcn(i2c_bus_desc, ADC_CHANNEL_0);

	if(adc0 < 0)
	{
		printf("Error in reading ADC Channel 0 register\n");
	}
	else 
		printf("Output of ADC Channel 0:%d", (int)adc0);

	adc1 = apds_9301_read_adcn(i2c_bus_desc, ADC_CHANNEL_1);

	if(adc1 < 0)
	{
		printf("Error in reading ADC Channel 1 register");
	}
	else
		printf("Output of ADC Channel 1: %d", (int)adc1);
	apds_9301_shutdown(i2c_bus_desc);
/*
	retval = i2c_bus_access(i2c_bus_desc, 0x39);

	if(retval < 0)
	{
		printf("Error accessing I2C slave device; %s", strerror(errno));
		return 1;
	}


	buf[0] =  0x00|0x80;   //Sending a command byte saying i want to access address 0x00 register
	buf[1] = 0x03;         //Data being written

	retval = i2c_bus_write(i2c_bus_desc, buf[0], buf+1, 1);   

        if(retval < 0)
        {
        	printf("Error writing I2C slave device; %s", strerror(errno));
                return 1;
        }


	retval = i2c_bus_read(i2c_bus_desc, buf[0], buf, 1);

        if(retval < 0)
        {
        	printf("Error reading from I2C slave device; %s", strerror(errno));
                return 1;
        }

	printf("\n\n%d\n", (int )buf[1]);*/
	close(i2c_bus_desc);

	return 0;
	
}

