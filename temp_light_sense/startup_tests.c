#include "include/apds_9301_driver.h"
#include "include/i2c_wrapper.h"
#include "include/tmp_102_driver.h"
#include "startup_tests.h"
#include <semaphore.h>
#include <errno.h>
#include "include/logger_task.h"

int test_light_sensor(int i2c_bus_desc)
{
    uint16_t thresh_low_reg = 25;
    int retval;

    retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);

    if(retval < 0)
    {
            //printf("Error accessing I2C slave device: %s", strerror(errno));
            i2c_bus_free();
            return -1;
    }

    retval = apds_9301_write_thresh_low_reg(i2c_bus_desc, thresh_low_reg);


    if(retval < 0)
    {
            //printf("\nLight Sensor is not working: %s\n", strerror(errno));
            i2c_bus_free();
            return -1;
    }
    retval = apds_9301_read_thresh_low_reg(i2c_bus_desc);

    if(retval < 0)
    {
            //printf("\nLight Sensor is not working: %s\n", strerror(errno));
            i2c_bus_free();
            return -1;
    }

	  thresh_low_reg = (uint16_t) (retval >> 8);

    if(thresh_low_reg == 25)
    {
            //printf("\nLight Sensor is functional\n");
            retval = 0;
    }
    else
    {
            //printf("\nThe light sensor is not functional: %s\n", strerror(errno));
            retval = -1;
    }

    i2c_bus_free();
    return retval;
}


int test_temp_sensor(int i2c_bus_desc)
{
	float temperature = 25.0;
	int retval;

  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);

  if(retval < 0)
  {
      //printf("Error accessing I2C slave device: %s", strerror(errno));
      i2c_bus_free();
      return -1;
  }

  retval = tmp_102_write_TLow_reg(i2c_bus_desc, temperature);

  if(retval < 0)
  {
      //printf("\nTemperature Sensor is not working: %s\n", strerror(errno));
      i2c_bus_free();
      return -1;
  }

	retval = tmp_102_read_TLow_reg(i2c_bus_desc, &temperature);

	if(retval < 0)
	{
		//printf("\nTemperature Sensor is not working: %s\n", strerror(errno));
		i2c_bus_free();
		return -1;
	}

	if(temperature == 25.0)
	{
                //printf("\nTemperature Sensor is functional\n");
                retval = 0;
	}

  else
	{
		//printf("\n The temperature sensor is not functional:%s\n", strerror(errno));
		retval = -1;
	}

  i2c_bus_free();
	return retval;
}

int startup_tests(mqd_t mq_heartbeat, bool *task_heartbeat, bool *sensor_alive)
{
	int i2c_bus_desc, retval;
	mq_payload_heartbeat_t heartbeat;
  char log_message[128];

	struct timespec abs_timeout;

	abs_timeout.tv_sec = 1;
	abs_timeout.tv_nsec = 0;

  i2c_bus_desc = i2c_bus_init(2);

  bzero(log_message, sizeof(log_message));
  sprintf(log_message, "## STARTUP TEST ##\nStart up tests are beginning.");
  LOG(mq_logger, log_message);

	retval = test_temp_sensor(i2c_bus_desc);
	if(retval < 0)
		sensor_alive[0] = false;
	else
		sensor_alive[0] = true;

  bzero(log_message, sizeof(log_message));
  sprintf(log_message, "## STARTUP TEST ## %s: %s.", "Temperature Sensor", sensor_alive[0]? "StartUp Succesful":"StartUp Failed");
  LOG(mq_logger, log_message);

	retval = test_light_sensor(i2c_bus_desc);
  if(retval < 0)
      sensor_alive[1] = false;

  else
      sensor_alive[1] = true;

  bzero(log_message, sizeof(log_message));
  sprintf(log_message, "## STARTUP TEST ## %s: %s.", "Light Sensor", sensor_alive[0]? "StartUp Succesful":"StartUp Failed");
  LOG(mq_logger, log_message);

  for(int i = 0; i<4; i++)
  {
      retval = mq_timedreceive(mq_heartbeat, (char *) &heartbeat, sizeof(heartbeat), NULL, &abs_timeout);

  	  if(retval > 0)
  	  {
  		task_heartbeat[heartbeat.sender_id] = heartbeat.heartbeat_status;
	      }
  }

  bzero(log_message, sizeof(log_message));
  sprintf(log_message, "## STARTUP TEST ## Receiving heartbeats...");
  LOG(mq_logger, log_message);


  for(int i = 0; i<4; i++)
  {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## STARTUP TEST ## %s: %s.", task_name[i], task_heartbeat[heartbeat.heartbeat_status] ? "StartUp Succesful":"StartUp Failed");
      LOG(mq_logger, log_message);
  }
}
