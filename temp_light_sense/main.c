#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <errno.h>

#include <string.h>

#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/ioctl.h>

#include <pthread.h>
#include <semaphore.h>


#include "include/i2c_wrapper.h"
#include "include/apds_9301_driver.h"
#include "include/tmp_102_driver.h"
#include "include/light_sense_task.h"
#include "include/temp_sense_task.h"
#include "include/logger_task.h"
#include "include/sock_comm_task.h"

//int bus_number = 2;
/*
int main()
{
	uint16_t word;
	int32_t adc0, adc1;
	int16_t timing_reg, control_reg, id_reg, thresh_low_reg, thresh_high_reg;
	char filename[20];
	int i2c_bus_desc;
	int retval, regaddr;
	uint8_t buf[10];

	float ambient_lux;

	regaddr = 0x52;

	i2c_bus_desc = i2c_bus_init(bus_number);

	if(i2c_bus_desc < 0)
	{
		printf("Error opening a i2c character device file: %s", strerror(errno));
		return 1;
	}

	apds_9301_init(i2c_bus_desc);
	usleep(100000);
//	while(1)
//	{
	adc0 = apds_9301_read_adcn(i2c_bus_desc, ADC_CHANNEL_0);

	if(adc0 < 0)
	{
		printf("Error in reading ADC Channel 0 register\n");
	}
	else
		printf("Output of ADC Channel 0:%d\n", (int)adc0);

	adc1 = apds_9301_read_adcn(i2c_bus_desc, ADC_CHANNEL_1);
	if(adc1 < 0)
	{
		printf("Error in reading ADC Channel 1 register\n");
	}
	else
		printf("Output of ADC Channel 1: %d\n", (int)adc1);
	//usleep(1000000);
//	}

	ambient_lux = calculate_ambient_lux((uint16_t)adc0, (uint16_t)adc1);

	printf("The Ambient Light is lux is %f", ambient_lux);

	printf("\nTiming Register:%02x", (uint8_t )apds_9301_read_timing_reg(i2c_bus_desc));

	printf("\n Control Register: %02x", (uint8_t)apds_9301_read_control_reg(i2c_bus_desc));

	printf("\n Id Register: %02x", (uint8_t)apds_9301_read_id_reg(i2c_bus_desc));
	printf("\n Threshold Low Register: %04x", (uint16_t)apds_9301_read_thresh_low_reg(i2c_bus_desc));
	printf("\n Threshold High Register: %04x", (uint16_t)apds_9301_read_thresh_high_reg(i2c_bus_desc));

	apds_9301_shutdown(i2c_bus_desc);


/*	retval = i2c_bus_access(i2c_bus_desc, 0x39);

	if(retval < 0)
	{
		printf("Error accessing I2C slave device; %s", strerror(errno));
		return 1;
	}*/

/*
	buf[0] =  0x01|0x80;   //Sending a command byte saying i want to access address 0x00 register
	buf[1] = 0x03;         //Data being written

	retval = i2c_bus_read(i2c_bus_desc, buf[0], buf, 1);

        if(retval < 0)
        {
        	printf("Error writing I2C slave device; %s", strerror(errno));
                return 1;
        }
	printf("\n\n%hhu%hhu\n", buf[1], buf[0]);*/

/*	retval = i2c_bus_read(i2c_bus_desc, buf[0], buf, 1);

        if(retval < 0)
        {
        	printf("Error reading from I2C slave device; %s", strerror(errno));
                return 1;
        }

	printf("\n\n%d\n", (int )buf[1]);*/
/*	close(i2c_bus_desc);

	return 0;

}
*/
/*
int bus_number = 2;
#include "include/tmp_102_driver.h"
#include <poll.h>

int GPIO_Interrupt_Setup()
{
	int retval, gpio_desc;
	char buf[10];
	gpio_desc = open("/sys/class/gpio/export", O_WRONLY);
	if(gpio_desc < 0)
	{
		printf("Error opening sys/class/gpio/export file. So can't reserve a GPIO pin%s", strerror(errno));
		return -1;
	}

	bzero(buf, sizeof(buf));
	sprintf(buf, "%d", GPIO_INTERRUPT_PIN);

	retval = write(gpio_desc, buf, strlen(buf));
	if(retval < 0)
	{
		printf("Error reserving GPIO%d pin: %s", GPIO_INTERRUPT_PIN, strerror(errno));
		gpio_desc = open("/sys/class/gpio/unexport", O_WRONLY);
		sprintf(buf, "%d", GPIO_INTERRUPT_PIN);
		retval = write(gpio_desc, buf, strlen(buf));
		return -1;
	}

	bzero(buf, sizeof(buf));

	sprintf(buf, "/sys/class/gpio/gpio%d/direction", GPIO_INTERRUPT_PIN);

	gpio_desc = open(buf, O_WRONLY);
	if(gpio_desc < 0)
	{
		printf("Error opening %s file. So can't set the direction of the GPIO pin");
		gpio_desc = open("/sys/class/gpio/unexport", O_WRONLY);
		sprintf(buf, "%d", GPIO_INTERRUPT_PIN);
		retval = write(gpio_desc, buf, strlen(buf));
		return -1;
	}
	bzero(buf, sizeof(buf));
	sprintf(buf, "in");
	retval = write(gpio_desc, buf, strlen(buf));
	if(retval < 0)
	{
                printf("Error opening %s file. So can't set the direction of the GPIO pin");
                gpio_desc = open("/sys/class/gpio/unexport", O_WRONLY);
                sprintf(buf, "%d", GPIO_INTERRUPT_PIN);
                retval = write(gpio_desc, buf, strlen(buf));
                return -1;
	}

        sprintf(buf, "/sys/class/gpio/gpio%d/edge", GPIO_INTERRUPT_PIN);

        gpio_desc = open(buf, O_WRONLY);
        if(gpio_desc < 0)
        {
                printf("Error opening %s file. So can't set the GPIO pin in edge triggered interrupt mode");
                gpio_desc = open("/sys/class/gpio/unexport", O_WRONLY);
                sprintf(buf, "%d", GPIO_INTERRUPT_PIN);
                retval = write(gpio_desc, buf, strlen(buf));
                return -1;
        }
        bzero(buf, sizeof(buf));
        sprintf(buf, "both");
        retval = write(gpio_desc, buf, strlen(buf));
        if(retval < 0)
        {
                printf("Error opening %s file. So can't set the GPIO pin in edge triggered interrupt mode");
                gpio_desc = open("/sys/class/gpio/unexport", O_WRONLY);
                sprintf(buf, "%d", GPIO_INTERRUPT_PIN);
                retval = write(gpio_desc, buf, strlen(buf));
                return -1;
        }
	return gpio_desc;

}

int GPIO_Interrupt_Scan_Thread(int i2c_bus_desc)
{
	int gpio_desc, retval;
	struct pollfd gpio_pollfd;
	float temperature;

	gpio_desc = GPIO_Interrupt_Setup();
	if(gpio_desc < 0)
	{
		return -1;
	}

	gpio_pollfd.fd = gpio_desc;
	gpio_pollfd.events = POLLPRI|POLLERR;

	while(1)
	{
		poll(&gpio_pollfd, 1, -1);
		if(gpio_pollfd.revents|POLLPRI)
		{
		        retval = tmp_102_read_temperature_reg(i2c_bus_desc, &temperature);
        	        printf("\nThe temperature is %2.4f", temperature);
		}
	}
}*/
/*
int main()
{

	int i2c_bus_desc, retval;
	uint16_t config_write, config_read;
	float temperature;

	config_write = 0;

	config_read = 150;

        i2c_bus_desc = i2c_bus_init(bus_number);

        if(i2c_bus_desc < 0)
        {
                printf("Error opening a i2c character device file: %s", strerror(errno));
                return 1;
        }

/*
	tmp_102_write_config_reg(i2c_bus_desc, config_write);
	tmp_102_read_config_reg(i2c_bus_desc, &config_read);

	printf("The config register is %04x It is written as %04x", config_write, config_read);
*/
/*	tmp_102_init(i2c_bus_desc);

	temperature = 30.0625;

	retval = tmp_102_write_TLow_reg(i2c_bus_desc, temperature);
	temperature = 0;
	retval = tmp_102_read_TLow_reg(i2c_bus_desc, &temperature);

	printf("\nThe Lower threshold for temperature is %2.4f", temperature);

	temperature = 40.0625;

	retval = tmp_102_write_THigh_reg(i2c_bus_desc, temperature);
	temperature = 0;
	retval = tmp_102_read_THigh_reg(i2c_bus_desc, &temperature);

	printf("\nThe Higher threshold for temperature is %2.4f", temperature);

//        i2c_bus_desc = i2c_bus_init(bus_number);

	retval = tmp_102_read_temperature_reg(i2c_bus_desc, &temperature);
	if(retval > 0)
	{
		printf("\nThe temperature is %2.4f", temperature);
	}
	GPIO_Interrupt_Scan_Thread(i2c_bus_desc);
	return 0;
}
*/

#include "main.h"

pthread_t logger_task;
pthread_t temp_sense_task;
pthread_t light_sense_task;
pthread_t sock_comm_task;

bool light_read = false;
bool temp_read = false;
bool temp_asynch = false;
bool light_asynch = false;

bool task_alive[4] = {true};
bool task_heartbeat[4] = {false};
bool send_heartbeat[4] = {true};

FILE *log_file;

char task_name[4][30];

struct sigevent heartbeat_sevp;

sem_t sem_light, sem_temp, sem_logger, sem_sock_comm;
mqd_t mq_light, mq_temp, mq_logger, mq_sock_comm, mq_heartbeat;


int main (int argc, char *argv[])
{
	timer_t timer_id;
  struct sigevent sevp;
	struct itimerspec tspec;
	pthread_args_t thread_args;
	int retval;
	mq_payload_heartbeat_t heartbeat;

	if (argc != 2)
  {
    printf ("USAGE: %s <log filename>\n", argv[0]);
    exit(1);
  }

  //Open the log file in the write mode
  log_file = fopen(argv[1], "w");
  if(log_file == NULL)
  {
    printf ("\n## ERROR ## Cannot create the file \"%s\". Exiting.\n", argv[1]);
    exit(1);
  }

	fclose(log_file);

	setup_mq();

	sem_init(&sem_logger, 0, 0);
	sem_init(&sem_light, 0, 0);
	sem_init(&sem_temp, 0, 0);
	sem_init(&sem_sock_comm, 0, 0);

	//Set the timer configuration
	sevp.sigev_notify = SIGEV_THREAD;
	sevp.sigev_value.sival_ptr = &timer_id;
	sevp.sigev_notify_function = timer_expiry_handler;
	sevp.sigev_notify_attributes = NULL;

	bzero(task_name, sizeof(task_name));
	strcpy(task_name[0], LIGHT_TASK_NAME);
	strcpy(task_name[1], TEMP_TASK_NAME);
	strcpy(task_name[2], LOGGER_TASK_NAME);
	strcpy(task_name[3], SOCK_COMM_TASK_NAME);

	//Set the timer value to 500ms
	tspec.it_value.tv_sec = 0;
	tspec.it_value.tv_nsec = TIMER_EXPIRY_MS *1000000;
	tspec.it_interval.tv_sec = 0;
	tspec.it_interval.tv_nsec = TIMER_EXPIRY_MS *1000000;

	//Timer creation and setting alarm
	timer_create(CLOCK_ID, &sevp, &timer_id);
	timer_settime(timer_id, 0, &tspec, 0);

	//pthread_create(&logger_task, NULL, logger_task_thread, (void *) &thread_args);
	pthread_create(&temp_sense_task, NULL, temp_sense_task_thread, (void *) &thread_args);
	pthread_create(&light_sense_task, NULL, light_sense_task_thread, (void *) &thread_args);
	pthread_create(&sock_comm_task, NULL, sock_comm_task_thread, (void *) &thread_args);

	while(1)
	{
		retval = mq_receive(mq_heartbeat, (char *) &heartbeat, sizeof(heartbeat), NULL);

		if(retval > 0)
			task_heartbeat[heartbeat.sender_id] = heartbeat.heartbeat_status;
		}
}


void timer_expiry_handler(union sigval arg)
{
	static uint8_t count = 0;

	if(count % 2 == 0)
	{
		count++;
		printf("Hello world\n");
		temp_read = true;
		sem_post(&sem_temp);
		sem_post(&sem_logger);
	}

	else
	{
		count ++;
		light_read = true;
		sem_post(&sem_light);
		sem_post(&sem_sock_comm);
	}

	if(count == 10)
	{
		count = 0;

		for(int i = 0; i < 4; i++)
		{
			if(task_alive[i])
			{
				if(task_heartbeat[i])
				{
					printf("\n\n\n\n\n%s is still Alive\n", task_name[i]);
					send_heartbeat[i] = true;
					task_alive[i] = true;
				}
				else
				{
					task_alive[i] = false;
					printf("\n\n\n\n\n%s is dead. There is no heartbeat on it.\n", task_name[i]);
				}
			}
		}
	}
}

void setup_mq(void)
{
	struct mq_attr attr;

	mq_unlink(MQ_LOGGER_ID);
	mq_unlink(MQ_HEARTBEAT_ID);
	mq_unlink(MQ_LIGHT_TASK_ID);
	mq_unlink(MQ_TEMP_TASK_ID);
	mq_unlink(MQ_SOCK_COMM_TASK_ID);

	bzero(&attr, sizeof(attr));
	attr.mq_flags = O_RDWR;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(mq_payload_heartbeat_t);

	mq_heartbeat = mq_open(MQ_HEARTBEAT_ID, O_CREAT | O_RDWR| O_NONBLOCK, S_IRUSR | S_IWUSR| S_IROTH| S_IWOTH, &attr);

	if(mq_heartbeat < 0)
		perror("Heartbeat Message Queue");

	attr.mq_msgsize = 64 * sizeof(char);

	mq_logger = mq_open(MQ_LOGGER_ID, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);

	if(mq_logger < 0)
		perror("Logger Queue");

	attr.mq_msgsize = sizeof(mq_temp_light_payload_t);

	mq_light = mq_open(MQ_LIGHT_TASK_ID, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);

	if(mq_light < 0)
		perror("Light Queue");

	mq_temp = mq_open(MQ_TEMP_TASK_ID, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);

	if(mq_temp < 0)
		perror("Temperature Queue");

	attr.mq_msgsize = 64 * sizeof(char);

	mq_sock_comm = mq_open(MQ_SOCK_COMM_TASK_ID, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);

	if(mq_sock_comm < 0)
		perror("Socket Responder Queue");
}
