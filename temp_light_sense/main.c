#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <errno.h>

#include <string.h>

#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/ioctl.h>

#include <pthread.h>
#include <semaphore.h>


#include "include/i2c_wrapper.h"
#include "include/apds_9301_driver.h"

//int bus_number = 2;
/*
int main()
{
	uint16_t word;
	int32_t adc0, adc1;
	int16_t timing_reg, control_reg, id_reg, thresh_low_reg, thresh_high_reg;
	char filename[20];
	int i2c_bus_desc;
	int retval, regaddr;
	uint8_t buf[10];

	float ambient_lux;

	regaddr = 0x52;

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

	ambient_lux = calculate_ambient_lux((uint16_t)adc0, (uint16_t)adc1);

	printf("The Ambient Light is lux is %f", ambient_lux);

	printf("\nTiming Register:%02x", (uint8_t )apds_9301_read_timing_reg(i2c_bus_desc));

	printf("\n Control Register: %02x", (uint8_t)apds_9301_read_control_reg(i2c_bus_desc));

	printf("\n Id Register: %02x", (uint8_t)apds_9301_read_id_reg(i2c_bus_desc));
	printf("\n Threshold Low Register: %04x", (uint16_t)apds_9301_read_thresh_low_reg(i2c_bus_desc));
	printf("\n Threshold High Register: %04x", (uint16_t)apds_9301_read_thresh_high_reg(i2c_bus_desc));

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
/*	close(i2c_bus_desc);

	return 0;

}
*/

/*#include "include/tmp_102_driver.h"
int main()
{

	int i2c_bus_desc, retval;
	uint16_t config_write, config_read;
	float temperature;

	config_write = 0;

	config_read = 150;

        i2c_bus_desc = i2c_bus_init(bus_number);

        if(i2c_bus_desc < 0)
        {
                printf("Error opening a i2c character device file: %s", strerror(errno));
                return 1;
        }


	tmp_102_write_config_reg(i2c_bus_desc, config_write);
	tmp_102_read_config_reg(i2c_bus_desc, &config_read);

	printf("The config register is %04x It is written as %04x", config_write, config_read);

	tmp_102_init(i2c_bus_desc);


        i2c_bus_desc = i2c_bus_init(bus_number);

	tmp_102_read_temperature_reg(i2c_bus_desc, &temperature);
	if(retval > 0)
	{
		printf("The temperature is %f", temperature);
	}

	return 0;
}*/

pthread_t logger_task;
pthread_t temp_sense_task;
pthread_t light_sense_task;
pthread_t sock_comm_task;

typedef pthread_args
{

} pthread_args_t;

int void main(void)
{
	pthread_args_t thread_args;

	pthread_create(&logger_task, NULL, logger_task_thread, (void *) &thread_args);
	pthread_create(&temp_sense_task, NULL, temp_sense_task_thread, (void *) &thread_args);
	pthread_create(&light_sense_task, NULL, light_sense_task_thread, (void *) &thread_args);
	pthread_create(&sock_comm_task, NULL, sock_comm_task_thread, (void *) &thread_args);

}
