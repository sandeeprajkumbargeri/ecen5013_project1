#include "../include/tmp_102_driver.h"
#include "../include/temp_sense_task.h"
#include "../include/i2c_wrapper.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdbool.h>

void *temp_sense_task_thread(void *args)
{
  int i2c_bus_desc, retval = 0;
  float temperature;
  mq_payload_t temp_heartbeat;

  i2c_bus_desc = i2c_bus_init(2);
  retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);

  if(retval < 0)
  {
      printf("Error accessing I2C slave device: %s", strerror(errno));
      pthread_exit(&retval);
  }

  tmp_102_init(i2c_bus_desc);

  retval = tmp_102_write_TLow_reg(i2c_bus_desc, THRESHOLD_LOW);
  if(retval < 0)
  {
      printf("Error accessing I2C slave device: %s", strerror(errno));
      pthread_exit(&retval);
  }

  retval = tmp_102_write_THigh_reg(i2c_bus_desc, THRESHOLD_HIGH);
  if(retval < 0)
  {
      printf("Error accessing I2C slave device: %s", strerror(errno));
      pthread_exit(&retval);
  }

  i2c_bus_free();

  bzero(temp_heartbeat.str, sizeof(temp_heartbeat.str));
  strcpy(temp_heartbeat.str, "TEMP_TASK Alive");

  while(1)
  {
    sem_wait(sem_temp);
    sem_post(sem_temp);

    if(temp_read == true)
    {
      sem_wait(sem_temp);
      retval = tmp_102_read_temperature_reg(i2c_bus_desc, &temperature);

      if(retval > 0)
    		printf("\nThe temperature is %2.4f", temperature);

      if(mq_send(mq_temp, (const char *) &temp_heartbeat, sizeof(data), 0) < 0)
      perror("TEMP TASK: Unable to send heartbeat.\n");
    }

    if(temp_asynch == true)
    {
      sem_post(sem_temp);
      //code for asynchronus request
    }
}
