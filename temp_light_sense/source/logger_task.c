#include "../include/logger_task.h"

void *logger_task_thread(void *args)
{
  char log_info[64];
  pthread_t logger_heartbeat;
  char current_time[32] = {0};

  log_file = fopen("out.log", "a");
  bzero(log_info, sizeof(log_info));

  pthread_create(&logger_heartbeat, NULL, heartbeat_notifier, (void *) NULL);

  while(1)
  {
    if(mq_receive(mq_logger, (char *) &log_info, sizeof(log_info), 0) < 0)
      errExit("## ERROR ## MQ Receive");

      update_time(current_time, sizeof(current_time));
      fprintf(log_file, "%s  %s\n", current_time, log_info);
  }
}

void update_time(char *current_time, size_t length)
{
  time_t present_time;
  struct tm *local_time;

  bzero(current_time, length);
  time(&present_time);
  local_time = localtime(&present_time);
  strcpy(current_time, asctime(local_time));
  current_time[strlen(current_time) - 1] = '\0';
}

void *heartbeat_notifier(void *args)
{
  mq_payload_heartbeat_t logger_heartbeat;

  bzero(&logger_heartbeat, sizeof(logger_heartbeat));
  logger_heartbeat.sender_id = LOGGER_TASK_ID;
  logger_heartbeat.heartbeat_status = true;

  while(1)
  {
    sem_wait(&sem_logger);

    if(send_heartbeat[LOGGER_TASK_ID])
    {
        if(mq_send(mq_heartbeat, (char *) &logger_heartbeat, sizeof(logger_heartbeat), 1) < 0)
            perror("\nLOGGER TASK: Unable to send heartbeat.\n");

        send_heartbeat[LOGGER_TASK_ID] = false;
    }
  }
}
