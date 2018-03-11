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
#include <time.h>
#include <math.h>
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
	uint8_t buf[10];

	float ambient_lux;

	regaddr = 0x52;

/*	retval = i2c_bus_read(i2c_bus_desc, 0x01|0x80, buf, 1);
	if(retval < 0)
	{
		printf("Errror reading contents of timing register");
	}
	printf("\n%hhu", buf[0]);*/

	i2c_bus_desc = i2c_bus_init(bus_number);

	if(i2c_bus_desc < 0)
	{
		printf("Error opening a i2c character device file: %s", strerror(errno));
		return 1;
	}

	apds_9301_init(i2c_bus_desc);
	usleep(100000);
//	while(1)
//	{	
	adc0 = apds_9301_read_adcn(i2c_bus_desc, ADC_CHANNEL_0);

	if(adc0 < 0)
	{
		printf("Error in reading ADC Channel 0 register\n");
	}
	else 
		printf("Output of ADC Channel 0:%d\n", (int)adc0);

	adc1 = apds_9301_read_adcn(i2c_bus_desc, ADC_CHANNEL_1);
	if(adc1 < 0)
	{
		printf("Error in reading ADC Channel 1 register\n");
	}
	else
		printf("Output of ADC Channel 1: %d\n", (int)adc1);
	//usleep(1000000);
//	}

	ambient_lux = calculate_ambient_lux(adc0, adc1);

	printf("The Ambient Light is lux is %f", ambient_lux);

	apds_9301_shutdown(i2c_bus_desc);
	

/*	retval = i2c_bus_access(i2c_bus_desc, 0x39);

	if(retval < 0)
	{
		printf("Error accessing I2C slave device; %s", strerror(errno));
		return 1;
	}*/

/*
	buf[0] =  0x01|0x80;   //Sending a command byte saying i want to access address 0x00 register
	buf[1] = 0x03;         //Data being written

	retval = i2c_bus_read(i2c_bus_desc, buf[0], buf, 1);   

        if(retval < 0)
        {
        	printf("Error writing I2C slave device; %s", strerror(errno));
                return 1;
        }
	printf("\n\n%hhu%hhu\n", buf[1], buf[0]);*/

/*	retval = i2c_bus_read(i2c_bus_desc, buf[0], buf, 1);

        if(retval < 0)
        {
        	printf("Error reading from I2C slave device; %s", strerror(errno));
                return 1;
        }

	printf("\n\n%d\n", (int )buf[1]);*/
	close(i2c_bus_desc);

	return 0;
	
}

