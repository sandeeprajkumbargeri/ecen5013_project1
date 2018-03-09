#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<err.h>
#include<errno.h>

#include<linux/i2c-dev.h>
#include<i2c/smbus.h>

int bus_number = 3;

int main()
{
	uint16_t word;
	char filename[20];
	int i2c_bus_desc;
	int retval, regaddr;

	regaddr = 0x52;

	filename = sprintf(filename, "/dev/i2c-%d", bus_number);	
	i2c_bus_desc = open(filename, O_RDWR);
	if(i2_bus_desc < 0)
	{
		printf("Error opening a i2c character device file: %s", strerror(errno));
		return 1;
	}

	retval = ioctl(i2c_bus_desc, I2C_SLAVE, 0x75);
	if(retval < 0)
	{
		printf("Error accessing I2C slave device; %s", strerror(errno));
		return 1;
	}

	i2c_smbus_write_word_data(i2c_bus_desc, 0x52);

	close(i2c_bus_desc);

	return 0;
	
}

