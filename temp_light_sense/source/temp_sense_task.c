#include "../include/tmp_102_driver.h"
#include "../include/i2c_wrapper.h"

void *temp_sense_task_thread(void *args)
{
  int i2c_bus_desc, retval = 0;
  float temperature;

  i2c_bus_desc = i2c_bus_init(2);
  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);

  if(retval < 0)
  {
      printf("Error accessing I2C slave device: %s", strerror(errno));
      return 1;
  }

  tmp_102_init(i2c_bus_desc);

  retval = tmp_102_write_TLow_reg(i2c_bus_desc, THRESHOLD_LOW);
  if(retval < 0)
  {
      printf("Error accessing I2C slave device: %s", strerror(errno));
      return 1;
  }

  retval = tmp_102_write_THigh_reg(i2c_bus_desc, THRESHOLD_HIGH);
  if(retval < 0)
  {
      printf("Error accessing I2C slave device: %s", strerror(errno));
      return 1;
  }

  i2c_bus_free();

  while(1)
  {
    retval = tmp_102_read_temperature_reg(i2c_bus_desc, &temperature);
  	if(retval > 0)
  	{
  		printf("\nThe temperature is %2.4f", temperature);
  	}
  }
}
