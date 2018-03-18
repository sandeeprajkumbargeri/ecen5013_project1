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

	uint8_t control_reg_data, timing_reg_data;

	if(retval < 0)
	{
		printf("Error accessing I2C slave device: %s", strerror(errno));
		return -1;
	}

	control_reg_data = POWER_UP;
	apds_9301_write_control_reg(i2c_bus_desc, control_reg_data);

	timing_reg_data = INTEGRATION_TIME|GAIN_MODE;
	apds_9301_write_timing_reg(i2c_bus_desc, timing_reg_data);

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

int16_t apds_9301_read_control_reg(int i2c_bus_desc)
{
	uint8_t buf;
	int retval;

	buf = COMMAND_BYTE|CONTROL_REGISTER;

        retval = i2c_bus_read(i2c_bus_desc, buf, &buf, 1);

        if(retval < 0)
        {
                printf("Error reading from I2C slave device; %s", strerror(errno));
                return -1;
        }
	return (int16_t )buf;

}
int apds_9301_write_control_reg(int i2c_bus_desc, uint8_t control_reg_data)
{
	uint8_t buf;
	int retval;

	buf = COMMAND_BYTE|CONTROL_REGISTER;

        retval = i2c_bus_write(i2c_bus_desc, buf, &control_reg_data, 1);

        if(retval < 0)
        {
                printf("Error powering up the Light Sensor: APDS 9301 %s", strerror(errno));
                return -1;
        }

        return 1;
}

int16_t apds_9301_read_timing_reg(int i2c_bus_desc)
{
	uint8_t buf;
	int retval;

        buf = COMMAND_BYTE|TIMING_REGISTER;

        retval = i2c_bus_read(i2c_bus_desc, buf, &buf, 1);

        if(retval < 0)
        {
                printf("Error setting the Integration Time and Gain mode %s", strerror(errno));
                return -1;
        }

        return (int16_t )buf;

}

int apds_9301_write_timing_reg(int i2c_bus_desc, uint8_t timing_reg_data)
{
        uint8_t buf;
	int retval;
        buf = COMMAND_BYTE|TIMING_REGISTER;

        retval = i2c_bus_write(i2c_bus_desc, buf, &timing_reg_data, 1);

        if(retval < 0)
        {
                printf("Error powering up the Light Sensor: APDS 9301 %s", strerror(errno));
                return -1;
        }

        return 1;
}

int16_t apds_9301_read_interrupt_reg(int i2c_bus_desc)
{
        uint8_t buf;
        int retval;

        buf = COMMAND_BYTE|INTERRUPT_REGISTER;

        retval = i2c_bus_read(i2c_bus_desc, buf, &buf, 1);

        if(retval < 0)
        {
                printf("Error setting the Integration Time and Gain mode %s", strerror(errno));
                return -1;
        }

        return (int16_t )buf;

}

int16_t apds_9301_read_id_reg(int i2c_bus_desc)
{
        uint8_t buf;
        int retval;

        buf = COMMAND_BYTE|ID_REGISTER;

        retval = i2c_bus_read(i2c_bus_desc, buf, &buf, 1);

        if(retval < 0)
        {
                printf("Error setting the Integration Time and Gain mode %s", strerror(errno));
                return -1;
        }

        return (int16_t )buf;

}


int apds_9301_write_interrupt_reg(int i2c_bus_desc, uint8_t interrupt_reg_data)
{
        uint8_t buf;
        int retval;
        buf = COMMAND_BYTE|INTERRUPT_REGISTER;

        retval = i2c_bus_write(i2c_bus_desc, buf, &interrupt_reg_data, 1);

        if(retval < 0)
        {
                printf("Error powering up the Light Sensor: APDS 9301 %s", strerror(errno));
                return -1;
        }

        return 1;
}

int32_t apds_9301_read_thresh_low_reg(int i2c_bus_desc)
{
        uint16_t thresh_low_reg;
        int retval;
        uint8_t buf[2];

        buf[0] = COMMAND_BYTE|THRESHLOWLOW_REGISTER|WORD_MODE;                 //Selects ADC0 if adc_number is 0 ad ADC1 if adc_number is 1
        retval = i2c_bus_read(i2c_bus_desc, buf[0], buf, 2);
        if(retval < 0)
        {
                printf("\nError reading contents of Lower Threshold Register: %s", strerror(errno));
                return retval;
        }

        //printf("\nlower Byte%02x HigherByte %02x\n", buf[0], buf[1]);

        thresh_low_reg = (uint16_t)buf[0] + (uint16_t)buf[1]<<8;     //Calculates ADC channel output using ADC Data Low Register and ADC Data High Register

        return (int32_t )thresh_low_reg;
}

int apds_9301_write_thresh_low_reg(int i2c_bus_desc, uint16_t thresh_low_reg)
{
        int retval;
        uint8_t buf[3];

        buf[0] = COMMAND_BYTE|THRESHLOWLOW_REGISTER|WORD_MODE;                 //Selects ADC0 if adc_number is 0 ad ADC1 if adc_number is 1
	buf[1] = (uint8_t )0xFF00^thresh_low_reg;
	buf[2] = (uint8_t )thresh_low_reg>>8;
        retval = i2c_bus_write(i2c_bus_desc, buf[0], buf+1, 2);
        if(retval < 0)
        {
                printf("\nError writing contents of Lower Threshold Register: %s", strerror(errno));
                return retval;
        }

        printf("\nlower Byte%02x HigherByte %02x\n", buf[1], buf[2]);

        return retval;
}

int32_t apds_9301_read_thresh_high_reg(int i2c_bus_desc)
{
        uint16_t thresh_high_reg;
        int retval;
        uint8_t buf[2];

        buf[0] = COMMAND_BYTE|THRESHHIGHLOW_REGISTER|WORD_MODE;                 //Selects ADC0 if adc_number is 0 ad ADC1 if adc_number is 1

        retval = i2c_bus_read(i2c_bus_desc, buf[0], buf, 2);
        if(retval < 0)
        {
                printf("\nError reading contents of Higher Threshold Register: %s", strerror(errno));
                return retval;
        }

        printf("\nlower Byte%02x HigherByte %02x\n", buf[0], buf[1]);

        thresh_high_reg = (uint16_t)buf[0] + (uint16_t)buf[1]<<8;     //Calculates ADC channel output using ADC Data Low Register and ADC Data High Register

        return (int32_t)thresh_high_reg;
}

int apds_9301_write_thresh_high_reg(int i2c_bus_desc, uint16_t thresh_high_reg)
{
        int retval;
        uint8_t buf[3];

        buf[0] = COMMAND_BYTE|THRESHHIGHLOW_REGISTER|WORD_MODE;                 //Selects ADC0 if adc_number is 0 ad ADC1 if adc_number is 1
        buf[1] = (uint8_t )0xFF00^thresh_high_reg;                         //Obtaining lower byte
        buf[2] = (uint8_t )thresh_high_reg>>8;			           //Obtaining higher byte
        retval = i2c_bus_write(i2c_bus_desc, buf[0], buf+1, 2);
        if(retval < 0)
        {
                printf("\nError writing contents of Lower Threshold Register: %s", strerror(errno));
                return retval;
        }

        printf("\nlower Byte%02x HigherByte %02x\n", buf[1], buf[2]);

        return retval;
}


int32_t apds_9301_read_adcn(int i2c_bus_desc, int adc_channel_number)
{
	int32_t adcn_channel_out;
	int retval;
	uint8_t buf[3], buf1[2];

	buf[0] = COMMAND_BYTE|(DATA0LOW_REGISTER+2*adc_channel_number)|WORD_MODE;                 //Selects ADC0 if adc_number is 0 ad ADC1 if adc_number is 1

//	printf("Address of ADC%d register %hhu", adc_channel_number, buf[0]);

	adcn_channel_out = i2c_bus_read(i2c_bus_desc, buf[0], buf, 2);
	if(adcn_channel_out < 0)
	{
		printf("\nError reading contents of ADC%d: %s\n", adc_channel_number, strerror(errno));
		return adcn_channel_out;
	}

	printf("\nlower Byte%02x\tHigherByte %02x\n", buf[0], buf[1]);

	adcn_channel_out = (uint16_t)buf[0] + (uint16_t)buf[1]<<8;     //Calculates ADC channel output using ADC Data Low Register and ADC Data High Register

	return adcn_channel_out;
}
