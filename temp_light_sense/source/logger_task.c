#include "../include/logger_task.h"

void *logger_task_thread(void *args)
{
  mq_payload_t log_info;
  FILE *log_file;
  char current_time[32] = {0};

  log_file = fopen("out.log", "w");
  bzero(log_info.str, sizeof(log_info.str));

  while(1)
  {
    if(mq_receive(mq_logger, (char *) &log_info.str, sizeof(log_info.str), 0) < 0)
      errExit("## ERROR ## MQ Receive");

      update_time(current_time, sizeof(current_time));
      fprintf(log_file, "%s  %s\n", current_time, buffer.log_info);
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
