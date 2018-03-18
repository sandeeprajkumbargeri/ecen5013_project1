#ifndef __STARTUP_TESTS_H
#define __STARTUP_TESTS_H

#include "main.h"

int test_light_sensor(int i2c_bus_desc);
int test_temp_sensor( int i2c_bus_desc);
int startup_tests(mqd_t mq_heartbeat, bool *task_heartbeat, bool *sensor_alive);

#endif
