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
#include <semaphore.h>

#include "../main.h"
#include "../include/sock_comm_task.h"

void *temp_sense_task_thread(void *args)
{
	int i2c_bus_desc, retval = 0;
  float temperature;
  mq_payload_heartbeat_t temp_heartbeat;
  struct mq_attr heartbeat_attr;
	comm_payload_t request;
	char response[64];

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

	bzero(&temp_heartbeat, sizeof(temp_heartbeat));
	temp_heartbeat.sender_id = TEMP_TASK_ID;
	temp_heartbeat.heartbeat_status = true;
	mq_getattr(mq_heartbeat, &heartbeat_attr);
	printf("\n \n \n %d \n\n\n%d", sizeof(temp_heartbeat), heartbeat_attr.mq_msgsize);

	while(1)
	{
		sem_wait(&sem_temp);
  	sem_post(&sem_temp);

		if(temp_read == true)
		{
			sem_wait(&sem_temp);
  		temp_read = false;
  		retval = tmp_102_read_temperature_reg(i2c_bus_desc, &temperature);

  		if(retval > 0)
			printf("\nThe temperature is %2.4f", temperature);

			if(send_heartbeat[TEMP_TASK_ID])
			{
      		if(mq_send(mq_heartbeat, (char *) &temp_heartbeat, sizeof(temp_heartbeat), 1) < 0)
						perror("\nTEMP TASK: Unable to send heartbeat.\n");

					send_heartbeat[TEMP_TASK_ID] = false;
			}


		}

		/*if(temp_asynch == true)
		{
  			sem_wait(&sem_temp);
				if(mq_receive(mq_temp, (char *) &request, sizeof(comm_payload_t), 0) < 0)
		      perror("Error receiving response");

				bzero(response, sizeof(response));
				service_request(request, response);
		}*/
	}
}


/*void service_request(comm_payload_t request, char *response)
{
	switch(request.command)
  {
    case COMMAND_TEMP_READ_TLOW:

        break;
    case COMMAND_TEMP_READ_THIGH:

      break;
    case COMMAND_TEMP_READ_DATA_REG:

      break;
    case COMMAND_TEMP_SET_SD_ON:

      break;
    case COMMAND_TEMP_SET_SD_OFF:

      break;
    case COMMAND_TEMP_READ_RESOLUTION:

      break;
    case COMMAND_TEMP_READ_FAULT_BITS:
    case COMMAND_TEMP_READ_EM:
    case COMMAND_TEMP_SET_EM_ON:
    case COMMAND_TEMP_SET_EM_OFF:

    case COMMAND_TEMP_SET_CONV_RATE_0:
    case COMMAND_TEMP_SET_CONV_RATE_1:
    case COMMAND_TEMP_SET_CONV_RATE_2:
    case COMMAND_TEMP_SET_CONV_RATE_3:
    case COMMAND_TEMP_READ_CONV_RATE:


    case COMMAND_LIGHT_SET_INTG_TIME_0:
    case COMMAND_LIGHT_SET_INTG_TIME_1:
    case COMMAND_LIGHT_SET_INTG_TIME_2:
    case COMMAND_LIGHT_READ_INTG_TIME:
    case COMMAND_LIGHT_READ_GAIN:
    case COMMAND_LIGHT_SET_GAIN_HIGH:
    case COMMAND_LIGHT_SET_GAIN_LOW:
    case COMMAND_LIGHT_SET_INT_ENABLE:
    case COMMAND_LIGHT_SET_INT_DISABLE:
    case COMMAND_LIGHT_READ_IDENTIFY_REG:
    case COMMAND_LIGHT_READ_INT_TRSHLD_LOW:
    case COMMAND_LIGHT_READ_INT_TRSHLD_HIGH:
    case COMMAND_LIGHT_SET_INT_TRSHLD_LOW:
    case COMMAND_LIGHT_SET_INT_TRSHLD_HIGH:
  }
}*/
