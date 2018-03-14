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


#define MQ_LOGGER_ID            "/logger_mq"
#define MQ_HEARTBEAT_ID         "/heartbeat_mq"
#define MQ_LIGHT_TASK_ID        "/light_mq"
#define MQ_TEMP_TASK_ID         "/temp_mq"
#define MQ_SOCK_COMM_TASK_ID    "/sock_comm_mq"

#define CLOCK_ID                CLOCK_MONOTONIC
#define TIMER_EXPIRY_MS         500

pthread_t logger_task;
pthread_t temp_sense_task;
pthread_t light_sense_task;
pthread_t sock_comm_task;

bool light_read = false;
bool temp_read = false;

sem_t *sem_light, *sem_temp, *sem_logger, *sem_sock_comm;
mqd_t mq_light, mq_temp, mq_logger, mq_sock_comm, mq_heartbeat;

void timer_expiry_handler(union sigval);

typedef struct pthread_args
{
	int32_t data;
} pthread_args_t;

//Structure of the data which is communicated between the parent and the child using pipes.
typedef struct payload
{
  char string[64];
} payload_t;

#endif
