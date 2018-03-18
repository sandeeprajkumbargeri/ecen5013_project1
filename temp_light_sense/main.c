#include "main.h"
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
#include "startup_tests.h"

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

pthread_t logger_task;
pthread_t temp_sense_task;
pthread_t light_sense_task;
pthread_t sock_comm_task;

bool light_read = false;
bool temp_read = false;
bool temp_asynch = false;
bool light_asynch = false;

bool sensor_alive[2] = {false};
bool task_alive[4] = {true};
bool task_heartbeat[4] = {false};
bool send_heartbeat[4] = {true};

char task_name[4][30];
char filename[30];

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
	char log_message[128] = {0};

	if (argc != 2)
  {
    printf ("USAGE: %s <log filename>\n", argv[0]);
    exit(1);
  }

	bzero(filename, sizeof(filename));
	strcpy(filename, argv[1]);
  //Open the log file in the write mode
  log_file = fopen(argv[1], "w");
	if(log_file == NULL)
  {
    printf ("\n## ERROR ## Cannot create the file \"%s\". Exiting.\n", argv[1]);
    exit(1);
  }

	fclose(log_file);

	for (int i = 0; i < 4; i++)
	{
		task_alive[i] = true;
		task_heartbeat[i] = false;
		send_heartbeat[i] = true;
	}

	sensor_alive[0] = false;
	sensor_alive[1] = false;

	setup_mq();
	bzero(log_message, sizeof(log_message));
	strcpy(log_message, "## MAIN ## Successfully setup message queues.");
	LOG(mq_logger, log_message);

	sem_init(&sem_logger, 0, 0);
	sem_init(&sem_light, 0, 0);
	sem_init(&sem_temp, 0, 0);
	sem_init(&sem_sock_comm, 0, 0);

	bzero(log_message, sizeof(log_message));
	strcpy(log_message, "## MAIN ## Successfully setup semaphores.");
	LOG(mq_logger, log_message);

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

	bzero(log_message, sizeof(log_message));
	sprintf(log_message, "## MAIN ## Successfully setup timer with %u sec %u msec expiry time.", TIMER_EXPIRY_S, TIMER_EXPIRY_MS);
	LOG(mq_logger, log_message);

	pthread_create(&logger_task, NULL, logger_task_thread, (void *) &thread_args);
	pthread_create(&temp_sense_task, NULL, temp_sense_task_thread, (void *) &thread_args);
	pthread_create(&light_sense_task, NULL, light_sense_task_thread, (void *) &thread_args);
	pthread_create(&sock_comm_task, NULL, sock_comm_task_thread, (void *) &thread_args);

	bzero(log_message, sizeof(log_message));
	strcpy(log_message, "## MAIN ## Successfully created all the threads.");
	LOG(mq_logger, log_message);

	startup_tests(mq_heartbeat, task_heartbeat, sensor_alive);

	tspec.it_value.tv_sec = TIMER_EXPIRY_S;
	tspec.it_value.tv_nsec = TIMER_EXPIRY_MS *1000000;
	tspec.it_interval.tv_sec = TIMER_EXPIRY_S;
	tspec.it_interval.tv_nsec = TIMER_EXPIRY_MS *1000000;

	//Timer creation and setting alarm
	timer_create(CLOCK_ID, &sevp, &timer_id);
	timer_settime(timer_id, 0, &tspec, 0);


	while(1)
	{
		retval = mq_receive(mq_heartbeat, (char *) &heartbeat, sizeof(heartbeat), NULL);

		if(retval > 0)
		{
			task_heartbeat[heartbeat.sender_id] = heartbeat.heartbeat_status;

			bzero(log_message, sizeof(log_message));
			sprintf(log_message, "## MAIN ## Received heartbeat from %s. Status: %s.", task_name[(uint8_t) heartbeat.sender_id], heartbeat.heartbeat_status? "good":"bad");
			LOG(mq_logger, log_message);
		}
	}
}


void timer_expiry_handler(union sigval arg)
{
	static uint8_t count = 0;
	char log_message[128] = {0};

	if(count % 2 == 0)
	{
		count++;
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
					bzero(log_message, sizeof(log_message));
					sprintf(log_message, "## MAIN ## %s is still alive.", task_name[i]);
					LOG(mq_logger, log_message);

					send_heartbeat[i] = true;
					task_alive[i] = true;
				}

				else
				{
					task_alive[i] = false;

					bzero(log_message, sizeof(log_message));
					sprintf(log_message, "## MAIN ## %s is dead. Failed to provide its heartbeat.", task_name[i]);
					LOG(mq_logger, log_message);
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

	attr.mq_msgsize = 128 * sizeof(char);

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
