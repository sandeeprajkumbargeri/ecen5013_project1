#include "../include/logger_task.h"

void *logger_task_thread(void *args)
{
  mqd_t mq;
  struct mq_attr attr;
  message_t buffer;
  FILE *log_file;
  char current_time[32] = {0};

  log_file = fopen("out.log", "w");
  mq_unlink("/message_queue");

  bzero(&attr, sizeof(attr));
  attr.mq_flags = O_RDWR;
  attr.mq_maxmsg = 100;
  attr.mq_msgsize = sizeof(message_t);

  mq = mq_open("/message_queue", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);

  if(mq < 0)
    errExit("## ERROR ## MQ Creation");


  while(1)
  {
    if(mq_receive(mq, (char *) &buffer, sizeof(message_t), 0) < 0)
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
