/*
# File_Name: unit_test.c
# Author:  Prithvi Teja Veeravalli <prithvi.veeravalli.colorado.edu>
           Sandeep Raj Kumbargeri (sandeep.kumbargeri@colorado.edu)
# Description: This program contains 8 unit tests for all 8 functions.
*/
#include <math.h>
#include <stdlib.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "apds_9301_driver.h"
#include "tmp_102_driver.h"
#include "i2c_wrapper.h"



#define TEST_SUCCESS 1

void test_apds_9301_init(void **state)
{
  int retval, i2c_bus_desc, bus_number = 2;
  int status;
  uint8_t control_reg;

  i2c_bus_desc = i2c_bus_init(bus_number);

  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);

  if(retval < 0)
  {
    status = 0;
  }

  else
  {
/*Testing starts from here*/
    apds_9301_init(i2c_bus_desc);    // When init is complete the control register reads 0x03 signifying it has been powered up. Therefor checking for the same

    retval = apds_9301_read_control_reg(i2c_bus_desc);
    if(retval < 0)
    {
      status = 0;
    }
    else
    {
      control_reg = (uint8_t) retval;
      if((control_reg&0x03) == 0x03)
      {
        status = 1;
      }
    }
  }
  apds_9301_shutdown(i2c_bus_desc);
  i2c_bus_free();
	assert_int_equal(TEST_SUCCESS, status);

}
void test_apds_9301_read_control_reg(void **state)
{
  int retval, i2c_bus_desc, bus_number = 2;
  int status;
  uint8_t control_reg;

  i2c_bus_desc = i2c_bus_init(bus_number);

  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);

  if(retval < 0)
  {
    status = 0;
  }

  else
  {
/*Testing starts from here*/

    control_reg = POWER_UP;
    retval = apds_9301_write_control_reg(i2c_bus_desc, control_reg);

    retval = apds_9301_read_control_reg(i2c_bus_desc);
    if(retval < 0)
    {
      status = 0;
    }
    else
    {
      control_reg = (uint8_t) retval;
      if((control_reg&POWER_UP) == 0x03)
      {
        status = 1;
      }
    }

    apds_9301_shutdown(i2c_bus_desc);
    i2c_bus_free();
          assert_int_equal(TEST_SUCCESS, status);
  }
}

void test_apds_9301_read_interrupt_reg(void **state)
{
  int retval, i2c_bus_desc, bus_number = 2;
  int status;
  //uint8_t control_reg;

  i2c_bus_desc = i2c_bus_init(bus_number);

  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);

  if(retval < 0)
  {
    status = 0;
  }

  else
  {
/*Testing starts from here*/
    apds_9301_init(i2c_bus_desc);    // When init is complete the control register reads 0x03 signifying it has been powered up. Therefor checking for the same

    retval = apds_9301_read_interrupt_reg(i2c_bus_desc);
    if(retval < 0)
    {
      status = 0;
    }
    else
    {
      status = 1;
    }
  }
  apds_9301_shutdown(i2c_bus_desc);
  i2c_bus_free();
	assert_int_equal(TEST_SUCCESS, status);

}
void test_tmp_102_init(void **state)
{
	int retval, status;
	int i2c_bus_desc;
  i2c_bus_desc = i2c_bus_init(2);
  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);

  if(retval < 0)
  {
      i2c_bus_free();
      status = 1;
  }

  retval = tmp_102_init(i2c_bus_desc);

	if(retval > 0)
	 status = 0;

  else
		status = 1;

  i2c_bus_free();
  assert_int_equal(TEST_SUCCESS, status);
}


void test_tmp_102_read_t_low_reg(void **state)
{
  float temperature = 25.0;
	int retval, status;
	int i2c_bus_desc;

	i2c_bus_desc = i2c_bus_init(2);
  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);

  if(retval < 0)
  {
      i2c_bus_free();
      status = -1;
  }

  retval = tmp_102_write_TLow_reg(i2c_bus_desc, temperature);

  if(retval < 0)
  {
      i2c_bus_free();
      status = -1;
  }

	retval = tmp_102_read_TLow_reg(i2c_bus_desc, &temperature);

	if(retval < 0)
	{
		i2c_bus_free();
		status = -1;
	}

	if(temperature == 25.0)
	 status = 1;

  else
		status = -1;

  i2c_bus_free();
  assert_int_equal(TEST_SUCCESS, status);
}

void test_tmp_102_read_t_high_reg(void **state)
{
  float temperature = 25.0;
	int retval, status, i2c_bus_desc;
	i2c_bus_desc = i2c_bus_init(2);
  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);

  if(retval < 0)
  {
      i2c_bus_free();
      status = -1;
  }

  retval = tmp_102_write_THigh_reg(i2c_bus_desc, temperature);

  if(retval < 0)
  {
      i2c_bus_free();
      status = -1;
  }

	retval = tmp_102_read_THigh_reg(i2c_bus_desc, &temperature);

	if(retval < 0)
	{
		i2c_bus_free();
		status = -1;
	}

	if(temperature == 25.0)
	 status = 1;

  else
		status = -1;

  i2c_bus_free();
  assert_int_equal(TEST_SUCCESS, status);
}

void test_tmp_102_read_t_data_reg(void **state)
{
  float temperature =  0;
	int retval, status, i2c_bus_desc;

	i2c_bus_desc = i2c_bus_init(2);
  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);

  if(retval < 0)
  {
      i2c_bus_free();
      status = -1;
  }

  retval = tmp_102_read_temperature_reg(i2c_bus_desc, &temperature);

  if(retval < 0)
  {
      i2c_bus_free();
      status = -1;
  }

	if(temperature > 0)
	 status = 1;

  else
		status = -1;

  i2c_bus_free();
  assert_int_equal(TEST_SUCCESS, status);
}

void test_tmp_102_read_config_reg(void **state)
{
  uint16_t conf_reg_value = 0x0400;
	int retval, status, i2c_bus_desc;
	i2c_bus_desc = i2c_bus_init(2);
  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);

  if(retval < 0)
  {
      i2c_bus_free();
      status = -1;
  }

  retval = tmp_102_write_config_reg(i2c_bus_desc, conf_reg_value);

  if(retval < 0)
  {
      i2c_bus_free();
      status = -1;
  }

	retval = tmp_102_read_config_reg(i2c_bus_desc, &conf_reg_value);

	if(retval < 0)
	{
		i2c_bus_free();
		status = -1;
	}

	if(conf_reg_value > 0)
	 status = 1;

  else
		status = -1;

  i2c_bus_free();
  assert_int_equal(TEST_SUCCESS, status);
}

int main(int argc, char **argv)
{



  const struct CMUnitTest unit_tests[] = {
    cmocka_unit_test(test_apds_9301_init),
    cmocka_unit_test(test_apds_9301_read_control_reg),
    cmocka_unit_test(test_apds_9301_read_interrupt_reg),
    cmocka_unit_test(test_tmp_102_init),
    cmocka_unit_test(test_tmp_102_read_t_low_reg),
    cmocka_unit_test(test_tmp_102_read_t_high_reg),
    cmocka_unit_test(test_tmp_102_read_t_data_reg),
    cmocka_unit_test(test_tmp_102_read_config_reg),
  };

  return cmocka_run_group_tests(unit_tests, NULL, NULL);
}
