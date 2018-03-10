#include "../include/apds_9301_driver.h"
#include "../include/i2c_wrapper.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

int apds_9301_init(int i2c_bus_desc)
{
	char buf[2];
	int retval;

	buf[0] = COMMAND_BYTE|CONTROL_REGISTER;
	buf[1] = POWER_UP;

	i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);
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

	i2c_bus_free();
	return 0;
}

int apds_9301_read_adc0(int i2c_bus_desc)
{

}
