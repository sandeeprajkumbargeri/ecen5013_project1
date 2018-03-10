#include "../include/apds_9301_driver.h"
#include "../include/i2c_wrapper.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

int apds_9301_init(int i2c_bus_desc)
{
	char buf[2];
	int retval;

        buf[0] = COMMAND_BYTE|TIMING_REGISTER;
        buf[1] = INTEGRATION_TIME|GAIN_MODE;

        retval = i2c_bus_write(i2c_bus_desc, buf[0], buf+1, 1);

        if(retval < 0)
        {
                printf("Error setting the Integration Time and Gain mode %s", strerror(errno));
                return 1;
        }

        retval = i2c_bus_read(i2c_bus_desc, buf[0], buf, 1);

        if(retval < 0)
        {
                printf("Error reading from I2C slave device; %s", strerror(errno));
                return 1;
        }


        printf("\n%d\n", (int ) buf[0]);


	buf[0] = COMMAND_BYTE|CONTROL_REGISTER;
	buf[1] = POWER_UP;

	retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);
	if(retval < 0)
	{
		printf("Error accessing I2C slave device: %s", strerror(errno));
		return 1;
	}
	retval = i2c_bus_write(i2c_bus_desc, buf[0], buf+1, 1);

	if(retval < 0)
	{
		printf("Error powering up the Light Sensor: APDS 9301 %s", strerror(errno));
		return 1;
	}

        retval = i2c_bus_read(i2c_bus_desc, buf[0], buf, 1);

        if(retval < 0)
        {
                printf("Error reading from I2C slave device; %s", strerror(errno));
                return 1;
        }


	printf("\n%d\n", (int ) buf[0]);

	i2c_bus_free();
	return 0;
}

int apds_9301_shutdown(int i2c_bus_desc)
{
	char buf[2];
	int retval;

	buf[0] = COMMAND_BYTE|CONTROL_REGISTER;
	buf[1] = POWER_DOWN;

	retval = i2c_bus_write(i2c_bus_desc, buf[0], buf + 1, 1);
	if(retval < 0)
	{
		printf("Error in shutdown of APDS 9301 sensor:%s", strerror(errno));
		return 1;
	}
	return 0;
}

uint16_t apds_9301_read_adcn(int i2c_bus_desc, int adc_channel_number)
{
	uint16_t adcn_channel_out;
	int retval;
	char buf[2];

	buf[0] = COMMAND_BYTE|WORD_MODE|(DATA0LOW_REGISTER+2*adc_channel_number);                 //Selects ADC0 if adc_number is 0 ad ADC1 if adc_number is 1
	
	retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);

	if(retval < 0)
	{
		printf("Error accessing I2C Slave Device %s", strerror(errno));
	}

	adcn_channel_out = i2c_bus_read(i2c_bus_desc, buf[0], buf, 2);
	if(adcn_channel_out < 0)
	{
		printf("Error reading contents of ADC%d: %s", adc_channel_number, strerror(errno));
	}
	else
		adcn_channel_out = (uint16_t)buf[0] + (uint16_t)buf[1]<<8;     //Calculates ADC channel output using ADC Data Low Register and ADC Data High Register
	i2c_bus_free();
	return adcn_channel_out;
}
