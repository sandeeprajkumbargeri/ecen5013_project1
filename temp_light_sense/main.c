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

int bus_number = 2;

int main()
{
	uint16_t word;
	char filename[20];
	int i2c_bus_desc;
	int retval, regaddr;
	char buf[10];

	regaddr = 0x52;

	sprintf(filename, "/dev/i2c-%d", bus_number);	
	i2c_bus_desc = open(filename, O_RDWR);
	if(i2c_bus_desc < 0)
	{
		printf("Error opening a i2c character device file: %s", strerror(errno));
		return 1;
	}

	retval = ioctl(i2c_bus_desc, I2C_SLAVE, 0x39);
	if(retval < 0)
	{
		printf("Error accessing I2C slave device; %s", strerror(errno));
		return 1;
	}

//	i2c_smbus_write_word_data(i2c_bus_desc, 0x52, 55);

	buf[0] =  0x00|0x80;
	buf[1] = 0xFF;

	retval = write(i2c_bus_desc, buf, 2);

        if(retval < 0)
        {
        	printf("Error writing I2C slave device; %s", strerror(errno));
                return 1;
        }


	retval = read(i2c_bus_desc, buf, 1);

        if(retval < 0)
        {
        	printf("Error reading from I2C slave device; %s", strerror(errno));
                return 1;
        }

	printf("\n\n%d", (int )buf[1]);
	close(i2c_bus_desc);

	return 0;
	
}

