#include "../include/i2c_wrapper.h"
#include "../include/apds_9301_driver.h"
#include "../main.h"
#include "../include/sock_comm_task.h"
#include "../include/light_sense_task.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdint.h>

#include <semaphore.h>

float calculate_ambient_lux(uint16_t adc_channel_out0, uint16_t adc_channel_out1)
{
        float ratio, ambient_lux;

        ratio = ((float)adc_channel_out1)/((float)adc_channel_out0);

        if((ratio>0) && (ratio<= 0.50))
        {
                (0.0304*adc_channel_out0) - (0.062*adc_channel_out0*(powf(ratio, 1.4)));
        }
        else if((ratio > 0.50)&&(ratio <= 0.61))
        {
                ambient_lux = (0.0224*adc_channel_out0) - (0.031*adc_channel_out1);
        }
        else if((ratio>0.61) && (ratio <= 0.80))
        {
                ambient_lux = (0.0128*adc_channel_out0) - (0.0153*adc_channel_out1);
        }
        else if((ratio > 0.80) && (ratio <= 1.30))
        {
                ambient_lux = (0.00146*adc_channel_out0) - (0.00112*adc_channel_out1);
        }
        else if(ratio > 1.30)
        {
                ambient_lux = 0;
        }
        return ambient_lux;

}

void *light_sense_task_thread(void *args)
{
	int i2c_bus_desc, bus_number, retval;
	int32_t adc0, adc1;
	float ambient_lux;
	mq_payload_heartbeat_t light_heartbeat;
	mq_temp_light_payload_t request;
	char response[64];

	printf("Devudaa");
	fflush(stdout);

	bus_number = 2;

	i2c_bus_desc = i2c_bus_init(bus_number);
	if(i2c_bus_desc < 0)
	{
		printf("Error opening a i2c character device file: %s", strerror(errno));
		fflush(stdout);
		retval = -1;
		pthread_exit(&retval);
	}

        retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);

	retval = apds_9301_init(i2c_bus_desc);

        i2c_bus_free();

	printf("I am here");

	if(retval < 0)
	{
		printf("Error setting up the ic_character device file: %s", strerror(errno));
		fflush(stdout);
		retval = -1;
		pthread_exit(&retval);
	}
	printf("I am also here");

  	bzero(&light_heartbeat, sizeof(light_heartbeat));
	light_heartbeat.sender_id = LIGHT_TASK_ID;
	light_heartbeat.heartbeat_status = true;


  	while(1)
	{
		sem_wait(&sem_light);
		sem_post(&sem_light);

		if(light_read)
		{
			sem_wait(&sem_light);
			light_read = false;
	        	retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);

			adc0 = apds_9301_read_adcn(i2c_bus_desc, ADC_CHANNEL_0);
			if(adc0 < 0)
			{
				printf("Error in reading ADC Channel 0 register\n");
				fflush(stdout);
			        i2c_bus_free();
				continue;
			}
			adc1 = apds_9301_read_adcn(i2c_bus_desc, ADC_CHANNEL_1);
			if(adc1 < 0)
			{
				printf("Error in reading ADC Channel 1 register\n");
				fflush(stdout);
                	        i2c_bus_free();
				continue;
			}
                	i2c_bus_free();

			ambient_lux = calculate_ambient_lux((uint16_t) adc0, (uint16_t) adc1);
			printf("The Ambient Light Lux is %2.4f\n", ambient_lux);
			fflush(stdout);

      			if(send_heartbeat[LIGHT_TASK_ID])
      			{
              			if(mq_send(mq_heartbeat, (char *) &light_heartbeat, sizeof(light_heartbeat), 1) < 0)
              			{
                      			perror("\nTEMP TASK: Unable to send heartbeat.\n");
              			}
              			send_heartbeat[LIGHT_TASK_ID] = false;
      			}

		}
		if(light_asynch == true)
		{
			sem_wait(&sem_light);

			if(mq_receive(mq_light, (char *)&request, sizeof(mq_temp_light_payload_t), 0) < 0)
				perror("\nError receiving response");
			bzero(response, sizeof(response));
			light_service_request(&request, response, i2c_bus_desc);
		}
	}
}


void light_service_request(mq_temp_light_payload_t *request, char *response, int i2c_bus_desc)
{
        uint16_t config_reg;
        int32_t read_value;
        uint8_t timing_reg;
        uint8_t value;
        int16_t timing_reg_retval;
        int16_t interrupt_reg_retval;
        uint8_t interrupt_reg;
        int16_t id_reg_retval;
        uint8_t id_reg;
        int16_t trshld_reg_retval;
        uint8_t trshld_reg;

        int retval;

        retval = i2c_bus_access(i2c_bus_desc, SLAVE_ADDRESS);
        if(retval < 0)
        {
                sprintf(response, "Error accessing I2C slave device: %s", strerror);
                pthread_exit(&retval);
        }

        switch(request->command)
        {
                case COMMAND_LIGHT_READ_INTG_TIME:

			timing_reg_retval = apds_9301_read_timing_reg(i2c_bus_desc);
			if(timing_reg_retval < 0)
			{
				sprintf(response, "Errror reading integration time: %s", strerror(errno));
				break;
			}
			timing_reg = (uint16_t) timing_reg_retval;
			timing_reg &= INTEGRATION_TIME_MASK;
			sprintf(response, "The Integration Time value is %d", timing_reg);
		break;
                case COMMAND_LIGHT_READ_GAIN:

                        timing_reg_retval = apds_9301_read_timing_reg(i2c_bus_desc);
                        if(timing_reg_retval < 0)
                        {
                                sprintf(response, "Errror reading Gain bit: %s", strerror(errno));
                                break;
                        }
                        timing_reg = (uint16_t) timing_reg_retval;
                        timing_reg = (timing_reg&GAIN_MASK)>>4;
                        sprintf(response, "The Gain bit value is %d", timing_reg);
		break;
                case COMMAND_LIGHT_SET_GAIN_HIGH:
                        if((request->command > COMMAND_LIGHT_SET_GAIN_LOW) && (request->command < COMMAND_LIGHT_SET_GAIN_HIGH))
                        {

                                value = request->command - COMMAND_LIGHT_SET_GAIN_LOW;

                                timing_reg_retval = apds_9301_read_timing_reg(i2c_bus_desc);

                                if(timing_reg_retval < 0)
                                {
                                        sprintf(response, "Error reading Gain bit: %s", strerror(errno));
                                        break;
                                }
                                timing_reg = (uint8_t) timing_reg_retval;
                                timing_reg = (timing_reg&(~GAIN_MASK))|(value<<4);

                                retval = apds_9301_write_timing_reg(i2c_bus_desc, timing_reg);
                                if(retval < 0)
                                {
                                        sprintf(response, "Error writing Gain bit: %s", strerror(errno));
                                        break;
                                }
                                sprintf(response, "The Gain bit in timing register is %d", value);
			}

		break;
                case COMMAND_LIGHT_SET_INT_ENABLE:
                        if((request->command > COMMAND_LIGHT_SET_INT_DISABLE) && (request->command < COMMAND_LIGHT_SET_INT_ENABLE))
                        {
                              value = request->command - COMMAND_LIGHT_SET_INT_DISABLE;

                                interrupt_reg_retval = apds_9301_read_interrupt_reg(i2c_bus_desc);

                                if(interrupt_reg_retval < 0)
                                {
                                        sprintf(response, "Error reading Interrupt bits: %s", strerror(errno));
                                        break;
                                }
                                interrupt_reg = (uint8_t) interrupt_reg_retval;
                                interrupt_reg = (interrupt_reg&(~INT_MASK))|(value<<4);

				                            retval = apds_9301_write_interrupt_reg(i2c_bus_desc, interrupt_reg);
                                if(retval < 0)
                                {
                                        sprintf(response, "Error writing Interrupt Bits: %s", strerror(errno));
                                        break;
                                }
                                sprintf(response, "The Interrupt bits in Interrupt register is %d", value);
                        }

		break;
    		case COMMAND_LIGHT_READ_IDENTIFY_REG:

			id_reg_retval = apds_9301_read_id_reg(i2c_bus_desc);
                        if(id_reg_retval < 0)
                        {
                                sprintf(response, "Errror reading id register: %s", strerror(errno));
                                break;
                        }
			timing_reg = (uint8_t) timing_reg_retval;
                        sprintf(response, "The ID of the sensor is %d", timing_reg);
		break;
    		case COMMAND_LIGHT_READ_INT_TRSHLD_LOW:

                        trshld_reg_retval = apds_9301_read_thresh_low_reg(i2c_bus_desc);
                        if(trshld_reg_retval < 0)
                        {
                                sprintf(response, "Errror reading Low register: %s", strerror(errno));
                                break;
                        }
			trshld_reg = (uint16_t)trshld_reg_retval;
                        sprintf(response, "The Low Threshold of the sensor is %d", trshld_reg);
		break;
    		case COMMAND_LIGHT_READ_INT_TRSHLD_HIGH:

                        trshld_reg_retval = apds_9301_read_thresh_high_reg(i2c_bus_desc);
                        if(trshld_reg_retval < 0)
                        {
                                sprintf(response, "Errror reading High Threshold register: %s", strerror(errno));
                                break;
                        }
                        trshld_reg = (uint16_t)trshld_reg_retval;
                        sprintf(response, "The High Threshold of the sensor is %d", trshld_reg);

		break;
    		case COMMAND_LIGHT_SET_INT_TRSHLD_LOW:
                        retval = apds_9301_write_thresh_low_reg(i2c_bus_desc, request->data);
                        if(retval < 0)
                        {
                                sprintf(response, "Errror writing Low Threshold register: %s", strerror(errno));
                                break;
                        }
                        sprintf(response, "The Low Threshold of the sensor has been set to %d", request->data);

		break;
    		case COMMAND_LIGHT_SET_INT_TRSHLD_HIGH:
                        retval = apds_9301_write_thresh_high_reg(i2c_bus_desc, request->data);
                        if(retval < 0)
                        {
                                sprintf(response, "Errror writing High Threshold register: %s", strerror(errno));
                                break;
                        }
                        sprintf(response, "The High Threshold of the sensor has been set to %d", request->data);
		break;

		default:
		        if((request->command > COMMAND_LIGHT_SET_INTG_TIME_0) && (request->command < COMMAND_LIGHT_SET_INTG_TIME_2))
        		{
        	        	value = request->command - COMMAND_LIGHT_SET_INTG_TIME_0;

        	        	timing_reg_retval = apds_9301_read_timing_reg(i2c_bus_desc);
        	        	if(timing_reg_retval < 0)
        	        	{
        	        	        sprintf(response, "Error reading integration time: %s", strerror(errno));
        	        	        break;
        	        	}
        	        	timing_reg = (uint8_t) timing_reg_retval;
        	        	timing_reg = (timing_reg&(~INTEGRATION_TIME_MASK))|(value);

        	        	retval = apds_9301_write_timing_reg(i2c_bus_desc, timing_reg);
        	        	if(retval < 0)
        	        	{
        	        	        sprintf(response, "Error writing integration time: %s", strerror(errno));
        	        	        break;
        	        	}
        	        	sprintf(response, "The Integration Time value is %d", value);
        	        	i2c_bus_free();
        	        	return ;
       			}
                        else if((request->command > COMMAND_LIGHT_SET_GAIN_LOW) && (request->command < COMMAND_LIGHT_SET_GAIN_HIGH))
                        {

                                value = request->command - COMMAND_LIGHT_SET_GAIN_LOW;

                                timing_reg_retval = apds_9301_read_timing_reg(i2c_bus_desc);
                                if(timing_reg_retval < 0)
                                {
                                        sprintf(response, "Error reading Gain bit: %s", strerror(errno));
                                        break;
                                }
                                timing_reg = (uint8_t) timing_reg_retval;
                                timing_reg = (timing_reg&(~GAIN_MASK))|(value<<4);

                                retval = apds_9301_write_timing_reg(i2c_bus_desc, timing_reg);
                                if(retval < 0)
                                {
                                        sprintf(response, "Error writing Gain bit: %s", strerror(errno));
                                        break;
                                }
                                sprintf(response, "The Gain bit in timing register is %d", value);
                        }


		break;
        }

        if(mq_send(mq_sock_comm, (char *) response, strlen(response), 1) < 0)
          sprintf(response, "Error in sending message.\n");

        i2c_bus_free();
	return ;
}
