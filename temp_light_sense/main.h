#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
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
#include <stdbool.h>
#include <time.h>
#include <sys/syscall.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>


#define MQ_LOGGER_ID            "/logger_mq"
#define MQ_HEARTBEAT_ID         "/heartbeat_mq"
#define MQ_LIGHT_TASK_ID        "/light_mq"
#define MQ_TEMP_TASK_ID         "/temp_mq"
#define MQ_SOCK_COMM_TASK_ID    "/sock_comm_mq"

#define LIGHT_TASK_ID	        0
#define TEMP_TASK_ID	        1
#define LOGGER_TASK_ID	        2
#define SOCK_COMM_TASK_ID       3

#define LIGHT_TASK_NAME		"Light Sensor Task"
#define TEMP_TASK_NAME		"Temperature Sesnor Task"
#define LOGGER_TASK_NAME	"Logger Task"
#define SOCK_COMM_TASK_NAME	"Socket COmmunication Task"

#define CLOCK_ID                CLOCK_MONOTONIC
#define TIMER_EXPIRY_S					1
#define TIMER_EXPIRY_MS         0

extern pthread_t logger_task;
extern pthread_t temp_sense_task;
extern pthread_t light_sense_task;
extern pthread_t sock_comm_task;

extern bool task_alive[4], task_heartbeat[4], send_heartbeat[4], sensor_alive[2];
extern char task_name[4][30];


extern bool light_read;
extern bool temp_read;
extern bool temp_asynch;
extern bool light_asynch;

extern struct sigevent heartbeat_sevp;

extern char filename[20];

extern sem_t sem_light, sem_temp, sem_logger, sem_sock_comm;
extern mqd_t mq_light, mq_temp, mq_logger, mq_sock_comm, mq_heartbeat;

void timer_expiry_handler(union sigval);
void heartbeat_msg_handler(union sigval);
void setup_mq(void);

typedef struct pthread_args
{
	int32_t data;
} pthread_args_t;

//Structure of the data which is communicated between the parent and the child using pipes.
typedef struct mq_payload_heartbeat
{
	char sender_id;
	bool heartbeat_status;
} mq_payload_heartbeat_t;

typedef struct mq_payload_comm
{
	unsigned char command;
	uint16_t data;
} mq_temp_light_payload_t;

FILE *log_file;

#endif
