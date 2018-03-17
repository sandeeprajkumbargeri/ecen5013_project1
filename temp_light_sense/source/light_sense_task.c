#include "../include/i2c_wrapper.h"
#include "../include/apds_9301_driver.h"
#include "../main.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <semaphore.h>

float calculate_ambient_lux(uint16_t adc_channel_out0, uint16_t adc_channel_out1)
{
        float ratio, ambient_lux;

        ratio = ((float)adc_channel_out1)/((float)adc_channel_out0);

        if((ratio>0) && (ratio<= 0.50))
        {
                (0.0304*adc_channel_out0)*(0.062*adc_channel_out0*(powf(ratio, 1.4)));
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
	}
}
