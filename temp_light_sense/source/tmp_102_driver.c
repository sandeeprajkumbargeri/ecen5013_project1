#include "../include/i2c_wrapper.h"
#include "../include/tmp_102_driver.h"

#include <unistd.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>


int tmp_102_init(int i2c_bus_desc)
{
        uint8_t buf[2];
        int retval;

        uint16_t config_reg;

        retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);
        if(retval < 0)
        {
                printf("Error accessing I2C slave device: %s", strerror(errno));
                return 1;
        }

        buf[0] = INTERRUPT_MODE|FAULT_QUEUE_COUNT;
	buf[1] = CONVERSION_RATE;

	config_reg = (buf[0]<<8) + (buf[1]);

        tmp_102_write_config_reg(i2c_bus_desc, config_reg);

        i2c_bus_free();
        return 0;
}
int tmp_102_read_temperature_reg(int i2c_bus_desc, float *temperature)
{
	uint8_t buf[2];
	int retval;
	int16_t temp_reg_dig_fmt;
	uint16_t *temp_ptr;

	temp_ptr = (uint16_t *)&temp_reg_dig_fmt;

	retval = i2c_bus_read(i2c_bus_desc, TEMPERATURE_REGISTER, buf, 2);
	if(retval < 0)
	{
                printf("\nError reading the Temp register of the Temp Sensor: TMP 102 %s", strerror(errno));
		return retval;
	}

	buf[1] = (buf[1]>>4) + ((buf[0]&0x0F)<<4);
	if(buf[0]&0x80)
	{
		buf[0] = ((buf[0]&0x70)>>4)|0xF8;
	}
	else
		buf[0] = (buf[0]&0x70)>>4;
		
	*temp_ptr = (uint16_t) (buf[1]) + (uint16_t)(buf[0]<<8);

	*temperature = temp_reg_dig_fmt*RESOLUTION;

	return retval;
}
int tmp_102_read_config_reg(int i2c_bus_desc, uint16_t *config_reg)
{
        uint8_t buf[2];
        int retval;

        retval = i2c_bus_read(i2c_bus_desc, CONFIG_REGISTER, buf, 2);
        if(retval < 0)
        {
                printf("\nError writing the config register of the Temp Sensor: TMP 102 %s", strerror(errno));
		return retval;
        }

	*config_reg = (buf[0] << 8) + buf[1];

        return retval;
	
}
int tmp_102_write_config_reg(int i2c_bus_desc, uint16_t config_reg)
{
	uint8_t buf[2];
	int retval;

	buf[1] = (uint8_t )config_reg&0x00FF;
	buf[0] = (uint8_t )config_reg >> 8;

	retval = i2c_bus_write(i2c_bus_desc, CONFIG_REGISTER, buf, 2);
	if(retval < 0)
	{
                printf("\nError writing the config register of the Temp Sensor: TMP 102 %s", strerror(errno));
	}
	return retval;
	
}
int tmp_102_read_TLow_reg(int i2c_bus_desc, float *TLow_reg)
{
        uint8_t buf[2];
        int retval;
        int16_t temp_reg_dig_fmt;
        uint16_t *temp_ptr;

        temp_ptr = (uint16_t *)&temp_reg_dig_fmt;

        retval = i2c_bus_read(i2c_bus_desc, TLOW_REGISTER, buf, 2);
        if(retval < 0)
        {
                printf("\nError reading the TLow register of the Temp Sensor: TMP 102 %s", strerror(errno));
                return retval;
        }

        buf[1] = (buf[1]>>4) + ((buf[0]&0x0F)<<4);
        if(buf[0]&0x80)
        {
                buf[0] = ((buf[0]&0x70)>>4)|0xF8;
        }
        else
                buf[0] = (buf[0]&0x70)>>4;

        *temp_ptr = (uint16_t) (buf[1]) + (uint16_t)(buf[0]<<8);

        *TLow_reg = temp_reg_dig_fmt*RESOLUTION;

        return retval;

}
int tmp_102_read_THigh_reg(int i2c_bus_desc, float *THigh_reg)
{
        uint8_t buf[2];
        int retval;
        int16_t temp_reg_dig_fmt;
        uint16_t *temp_ptr;

        temp_ptr = (uint16_t *)&temp_reg_dig_fmt;

        retval = i2c_bus_read(i2c_bus_desc, THIGH_REGISTER, buf, 2);
        if(retval < 0)
        {
                printf("\nError reading the THigh register of the Temp Sensor: TMP 102 %s", strerror(errno));
                return retval;
        }

        buf[1] = (buf[1]>>4) + ((buf[0]&0x0F)<<4);
        if(buf[0]&0x80)
        {
                buf[0] = ((buf[0]&0x70)>>4)|0xF8;
        }
        else
                buf[0] = (buf[0]&0x70)>>4;

        *temp_ptr = (uint16_t) (buf[1]) + (uint16_t)(buf[0]<<8);

        *THigh_reg = temp_reg_dig_fmt*RESOLUTION;

        return retval;
}
int tmp_102_write_TLow_reg(int i2c_bus_desc, float TLow_register)
{
        uint8_t buf[2];
        int retval;
        int16_t temp_reg_dig_fmt;
        uint16_t *temp_ptr;

        temp_ptr = (uint16_t *)&temp_reg_dig_fmt;

	temp_reg_dig_fmt = (int16_t)(TLow_register/RESOLUTION);

	buf[0] = ((uint8_t)((*temp_ptr&0x0F00)>>4) ) + ((uint8_t)((*temp_ptr&0x00F0)>>4));
	buf[1] = (*temp_ptr&0x000F)<<4;

        retval = i2c_bus_write(i2c_bus_desc, TLOW_REGISTER, buf, 2);
        if(retval < 0)
        {
                printf("\nError writing the TLow register of the Temp Sensor: TMP 102 %s", strerror(errno));
                return retval;
        }
        return retval;

}
int tmp_102_write_THigh_reg(int i2c_bus_desc, float THigh_register)
{
        uint8_t buf[2];
        int retval;
        int16_t temp_reg_dig_fmt;
        uint16_t *temp_ptr;

        temp_ptr = (uint16_t *)&temp_reg_dig_fmt;

        temp_reg_dig_fmt = (int16_t)(THigh_register/RESOLUTION);

        buf[0] = ((uint8_t)((*temp_ptr&0x0F00)>>4) ) + ((uint8_t)((*temp_ptr&0x00F0)>>4));
        buf[1] = (*temp_ptr&0x000F)<<4;

        retval = i2c_bus_write(i2c_bus_desc, THIGH_REGISTER, buf, 2);
        if(retval < 0)
        {
                printf("\nError writing the THigh register of the Temp Sensor: TMP 102 %s", strerror(errno));
                return retval;
        }
        return retval;

}

