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
	mq_temp_light_payload_t request;
	char response[64];
	char log_message[128];

	bzero(log_message, sizeof(log_message));
	strcpy(log_message, "Entered temp sense task");

	if(mq_send(mq_logger, (const char *) log_message, sizeof(log_message), 0) < 0)
		perror("Error Sending Request to Temp Task");

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
/*  			temp_read = false;
		        retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);
  			retval = tmp_102_read_temperature_reg(i2c_bus_desc, &temperature);
		        i2c_bus_free();

  			if(retval > 0)
				printf("\nThe temperature is %2.4f", temperature);
*/
			if(send_heartbeat[TEMP_TASK_ID])
			{
      				if(mq_send(mq_heartbeat, (char *) &temp_heartbeat, sizeof(temp_heartbeat), 1) < 0)
				perror("\nTEMP TASK: Unable to send heartbeat.\n");

				send_heartbeat[TEMP_TASK_ID] = false;
			}

		}

		if(temp_asynch == true)
		{
  			sem_wait(&sem_temp);

			if(mq_receive(mq_temp, (char *) &request, sizeof(mq_temp_light_payload_t), 0) < 0)
		      		perror("\nError receiving response");

			bzero(response, sizeof(response));
			service_request(&request, response, i2c_bus_desc);
		}
	}
}


void service_request(mq_temp_light_payload_t *request, char *response, int i2c_bus_desc)
{
	float TLow_reg, THigh_reg, temperature;
	uint16_t config_reg;
	uint8_t read_value;
	int retval;
	int length = 500;

        retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS_TMP);
        if(retval < 0)
        {
            printf("Error accessing I2C slave device: %s", strerror);
						pthread_exit(&retval);
				}

	switch(request->command)
	{
    		case COMMAND_TEMP_READ_TLOW:
									tmp_102_read_TLow_reg(i2c_bus_desc, &TLow_reg);
									sprintf(response, "T-Low Reg = %f.\n", TLow_reg);
        					break;
    		case COMMAND_TEMP_READ_THIGH:
                        tmp_102_read_TLow_reg(i2c_bus_desc, &TLow_reg);
												sprintf(response, "T-High Reg = %f.\n", TLow_reg);
      		break;
    		case COMMAND_TEMP_READ_DATA_REG:
                        retval = tmp_102_read_temperature_reg(i2c_bus_desc, &temperature);
                        if(retval > 0)
                        sprintf(response, "\nThe temperature is %2.4f", temperature);
      		break;
    		case COMMAND_TEMP_SET_SD_ON:
                        retval = tmp_102_read_config_reg(i2c_bus_desc, &config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError setting SD mode on%s\n", strerror(errno));
                                break;
                        }
                        config_reg = config_reg|SD_MASK;

                        retval = tmp_102_write_config_reg(i2c_bus_desc, config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError setting SD mode on%s\n", strerror(errno));
                                break;
                        }
                        sprintf(response, "\nSD mode has been switced on\n");

      		break;
    		case COMMAND_TEMP_SET_SD_OFF:
                        retval = tmp_102_read_config_reg(i2c_bus_desc, &config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError setting SD mode off%s\n", strerror(errno));
                                break;
                        }
                        config_reg = config_reg&(~SD_MASK);

                        retval = tmp_102_write_config_reg(i2c_bus_desc, config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError setting SD mode off%s\n", strerror(errno));
                                break;
                        }
                        sprintf(response, "\nSD mode has been switced off\n");

      		break;
    		case COMMAND_TEMP_READ_RESOLUTION:
			retval = tmp_102_read_config_reg(i2c_bus_desc, &config_reg);
			if(retval < 0)
			{
				sprintf(response, "\nError reading resolution bits%s\n", strerror(errno));
				break;
			}
			read_value = (uint8_t)( (config_reg&RESOLUTION_MASK) >> 8);
			sprintf(response, "\nThe converter resolution is %d\n", read_value);
      		break;
    		case COMMAND_TEMP_READ_FAULT_BITS:
                        retval = tmp_102_read_config_reg(i2c_bus_desc, &config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError reading fault bits%s\n", strerror(errno));
                                break;
                        }
                        read_value = (uint8_t)( (config_reg&FAULT_BITS_MASK) >> 8);
                        sprintf(response, "\nThe Fault Bits are %d\n", read_value);

		break;
    		case COMMAND_TEMP_READ_EM:
                        retval = tmp_102_read_config_reg(i2c_bus_desc, &config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError reading EM%s\n", strerror(errno));
                                break;
                        }
                        read_value = (uint8_t)( (config_reg&EM_MASK) >> 8);
                        sprintf(response, "\nThe EM is %d\n", read_value);
		break;
    		case COMMAND_TEMP_SET_EM_ON:
                        retval = tmp_102_read_config_reg(i2c_bus_desc, &config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError setting SD mode on%s\n", strerror(errno));
                                break;
                        }
                        config_reg = config_reg|(EM_MASK);

                        retval = tmp_102_write_config_reg(i2c_bus_desc, config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError setting SD mode on%s\n", strerror(errno));
                                break;
			}
                        sprintf(response, "\nSD mode has been switced on\n");

		break;
    		case COMMAND_TEMP_SET_EM_OFF:
                        retval = tmp_102_read_config_reg(i2c_bus_desc, &config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError setting SD mode on%s\n", strerror(errno));
                                break;
                        }
                        config_reg = config_reg&(~EM_MASK);

                        retval = tmp_102_write_config_reg(i2c_bus_desc, config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError setting SD mode on%s\n", strerror(errno));
                                break;
                        }
                        sprintf(response, "\nSD mode has been switced on\n");

		break;

    		case COMMAND_TEMP_READ_CONV_RATE:
                        retval = tmp_102_read_config_reg(i2c_bus_desc, &config_reg);
                        if(retval < 0)
                        {
                                sprintf(response, "\nError reading conversion rate %s\n", strerror(errno));
                                break;
                        }
                        read_value = (uint8_t)( (config_reg&CONV_RATE_MASK) >> 8);
                        sprintf(response, "\nThe conversion rate is %d\n", read_value);
		break;
		default:
			if((request->command > COMMAND_TEMP_SET_CONV_RATE_0)&&(request->command < COMMAND_TEMP_SET_CONV_RATE_3))
			{
                        	retval = tmp_102_read_config_reg(i2c_bus_desc, &config_reg);
                        	if(retval < 0)
                        	{
                        	        sprintf(response, "\nError setting Conversion rate %s\n", strerror(errno));
                        	        break;
                        	}
                        	config_reg = config_reg&(~CONV_RATE_MASK);
													config_reg = config_reg|((request->command - COMMAND_TEMP_SET_CONV_RATE_0)<<CONV_RATE_SHIFT);

                        	retval = tmp_102_write_config_reg(i2c_bus_desc, config_reg);
                        	if(retval < 0)
                        	{
                        	        sprintf(response, "\nError setting Conversion rate %s\n", strerror(errno));
                        	        break;
                        	}
                        	sprintf(response, "\nSetting Conversion Rate succesfull\n");

			}
		break;
	}
				if(mq_send(mq_sock_comm, (char *) response, strlen(response), 1) < 0)
					sprintf(response, "Error in sending message.\n");


				i2c_bus_free();


/*    		case COMMAND_LIGHT_SET_INTG_TIME_0:
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
    case COMMAND_LIGHT_SET_INT_TRSHLD_HIGH:*/
}
