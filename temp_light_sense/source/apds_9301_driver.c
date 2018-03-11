#include "../include/apds_9301_driver.h"
#include "../include/i2c_wrapper.h"
#include <unistd.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <math.h>

int apds_9301_init(int i2c_bus_desc)
{
	uint8_t buf[2];
	int retval;

	retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);
	if(retval < 0)
	{
		printf("Error accessing I2C slave device: %s", strerror(errno));
		return 1;
	}

        buf[0] = COMMAND_BYTE|CONTROL_REGISTER;
        buf[1] = POWER_UP;

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


        printf("\n%hhu\n",  buf[0]);


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


        printf("\n%hhu\n", buf[0]);

// 	apds_9301_read_adcn(i2c_bus_desc,0);

	i2c_bus_free();
	return 0;
}

int apds_9301_shutdown(int i2c_bus_desc)
{
	uint8_t buf[2];
	int retval;
	buf[0] = COMMAND_BYTE|CONTROL_REGISTER;
	buf[1] = POWER_DOWN;

        retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);
        if(retval < 0)
        {
                printf("Error accessing I2C slave device: %s", strerror(errno));
                return 1;
        }

	retval = i2c_bus_write(i2c_bus_desc, buf[0], buf + 1, 1);
	if(retval < 0)
	{
		printf("Error in shutdown of APDS 9301 sensor:%s", strerror(errno));
		return 1;
	}

        i2c_bus_free();

	return 0;
}

uint16_t apds_9301_read_adcn(int i2c_bus_desc, int adc_channel_number)
{
	uint16_t adcn_channel_out;
	int retval;
	uint8_t buf[3], buf1[2];

	buf[0] = COMMAND_BYTE|(DATA0LOW_REGISTER+2*adc_channel_number)|WORD_MODE;                 //Selects ADC0 if adc_number is 0 ad ADC1 if adc_number is 1
	
	printf("Address of ADC%d register %hhu", adc_channel_number, buf[0]);

	retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);

	usleep(1000000);

	if(retval < 0)
	{
		printf("\nError accessing I2C Slave Device %s", strerror(errno));
	}

	adcn_channel_out = i2c_bus_read(i2c_bus_desc, buf[0], buf, 2);
	if(adcn_channel_out < 0)
	{
		printf("\nError reading contents of ADC%d: %s", adc_channel_number, strerror(errno));
		return adcn_channel_out;
	}

	printf("\nlower Byte%02x HigherByte %02x", buf[0], buf[1]);

	adcn_channel_out = (uint16_t)buf[0] + (uint16_t)buf[1]<<8;     //Calculates ADC channel output using ADC Data Low Register and ADC Data High Register

        buf[0] = COMMAND_BYTE|CONTROL_REGISTER;
//        buf[1] = POWER_UP;

        retval = i2c_bus_read(i2c_bus_desc, buf[0], buf, 1);

        if(retval < 0)
        {
                printf("Error reading from I2C slave device; %s", strerror(errno));
                return 1;
        }


        printf("\nControl Register :%hhu\n",  buf[0]);


	i2c_bus_free();
	return adcn_channel_out;
}

float calculate_ambient_lux(uint16_t adc_channel_out0, uint16_t adc_channel_out1)
{
	float ratio, ambient_lux;

	ratio = ((float)adc_channel_out1)/((float)adc_channel_out0);

	if((ratio>0) && (ratio<= 0.50))
	{
		(0.0304*adc_channel_out0)*(0.062*adc_channel_out0*(powf(ratio, 1.4)));
	}	
	else if((ratio > 0.50)&&(ratio <= 0.61))
	{
		ambient_lux = (0.0224*adc_channel_out0) - (0.031*adc_channel_out1);
	}
	else if((ratio>0.61) && (ratio <= 0.80))
	{
		ambient_lux = (0.0128*adc_channel_out0) - (0.0153*adc_channel_out1);
	}
	else if((ratio > 0.80) && (ratio <= 1.30))
	{
		ambient_lux = (0.00146*adc_channel_out0) - (0.00112*adc_channel_out1);
	}
	else if(ratio > 1.30)
	{
		ambient_lux = 0;
	}
	return ambient_lux;

}
