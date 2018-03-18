/*
# File_Name: unit_test.c
# Author:  Prithvi Teja Veeravalli <prithvi.veeravalli.colorado.edu>
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
  uint8_t control_reg;

  i2c_bus_desc = i2c_bus_init(bus_number);

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
    if(control_reg == 0x03)
    {
      status = 1;
    }
  }

  apds_9301_shutdown(i2c_bus_desc);

	assert_int_equal(TEST_SUCCESS, status);
}


int main(int argc, char **argv)
{



  const struct CMUnitTest unit_tests[] = {
    cmocka_unit_test(test_apds_9301_init),
  /*  cmocka_unit_test(test_insert_at_end),
    cmocka_unit_test(test_insert_at_position),
    cmocka_unit_test(test_remove_at_beginning),
    cmocka_unit_test(test_remove_at_end),
    cmocka_unit_test(test_remove_at_position),
    cmocka_unit_test(test_size),
    cmocka_unit_test(test_destroy), */

/*    cmocka_unit_test(test_real_roots),
    cmocka_unit_test(test_imaginary_roots),
    cmocka_unit_test(test_complex_roots_skip),
    cmocka_unit_test(test_complex_roots_xfail),*/
  };

  return cmocka_run_group_tests(unit_tests, NULL, NULL);
}
