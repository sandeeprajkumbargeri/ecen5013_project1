#include "../main.h"
#include "../include/sock_comm_task.h"
#include "../include/logger_task.h"
#include <errno.h>


void *sock_comm_task_thread(void *args)
{
  int remote_sock = 0;
  char sockaddr_path[32];
  char response[64];
  struct sockaddr_un app_sockaddr, remote_sockaddr;
  socklen_t sockaddr_length;
  mq_temp_light_payload_t request;
  mq_payload_heartbeat_t sock_comm_heartbeat;
  char log_message[128];
  pthread_t sock_comm_heartbeat_notifier_desc;

  bzero(&sock_comm_heartbeat, sizeof(sock_comm_heartbeat));
  sock_comm_heartbeat.sender_id = LOGGER_TASK_ID;
  sock_comm_heartbeat.heartbeat_status = true;


  remote_sock = socket(AF_UNIX, SOCK_DGRAM, 0);     //create a datagram socket

  if(remote_sock < 0)
  {
    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Error Creating Socket. %s", strerror(errno));
    LOG(mq_logger, log_message);
  }

  bzero(sockaddr_path, sizeof(sockaddr_path));
  sprintf(sockaddr_path, "/tmp/temp_light_sense_socket");
  remove(sockaddr_path);

  sockaddr_length = sizeof(remote_sockaddr);
  bzero(&app_sockaddr, sockaddr_length);
  bzero(&remote_sockaddr, sockaddr_length);

  remote_sockaddr.sun_family = AF_UNIX;
  strncpy(remote_sockaddr.sun_path, sockaddr_path, sizeof(remote_sockaddr.sun_path) -1);

  if(bind(remote_sock, (struct sockaddr *) &remote_sockaddr, sizeof(struct sockaddr_un)) < 0)
  bzero(log_message, sizeof(log_message));
  sprintf(log_message, "## SOCK COMM ## Bind failed. %s", strerror(errno));
  LOG(mq_logger, log_message);

  if(mq_send(mq_heartbeat, (char *) &sock_comm_heartbeat, sizeof(sock_comm_heartbeat), 1) < 0)
  {
    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Unable to send heartbeat. %s", strerror(errno));
    LOG(mq_logger, log_message);
  }

  pthread_create(&sock_comm_heartbeat_notifier_desc, NULL, sock_comm_heartbeat_notifier, (void *) NULL);

  while(1)
  {
    bzero(&request, sizeof(request));

    if(recvfrom(remote_sock, (void *) &request, sizeof(request), 0, (struct sockaddr *) &app_sockaddr, &sockaddr_length) < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error receiving request from external application. %s", strerror(errno));
      LOG(mq_logger, log_message);
    }

    bzero(log_message, sizeof(log_message));
    sprintf(log_message, "## SOCK COMM ## Received request from external application. %s", strerror(errno));
    LOG(mq_logger, log_message);

    if(request.command > 0x00 && request.command < 0x10)
    {
      if(mq_send(mq_temp, (const char *) &request, sizeof(mq_temp_light_payload_t), 0) < 0)
      {
        bzero(log_message, sizeof(log_message));
        sprintf(log_message, "## SOCK COMM ## Error sending request to temp task. %s", strerror(errno));
        LOG(mq_logger, log_message);
      }

      temp_asynch = true;
      sem_post(&sem_temp);
    }

    if(request.command > 0x0F && request.command < 0x1E)
    {
      if(mq_send(mq_light, (const char *) &request, sizeof(mq_temp_light_payload_t), 0) < 0)
      {
        bzero(log_message, sizeof(log_message));
        sprintf(log_message, "## SOCK COMM ## Error sending request to light task. %s", strerror(errno));
        LOG(mq_logger, log_message);
      }

      light_asynch = true;
      sem_post(&sem_light);
    }

    bzero(response, sizeof(response));

    if(mq_receive(mq_sock_comm, (char *) response, sizeof(response), 0) < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error receiving response from temp/light task. %s", strerror(errno));
      LOG(mq_logger, log_message);
    }

    if(sendto(remote_sock, (const void *) response, sizeof(response), 0, (const struct sockaddr *) &app_sockaddr, sockaddr_length) < 0)
    {
      bzero(log_message, sizeof(log_message));
      sprintf(log_message, "## SOCK COMM ## Error sending response to external application. %s", strerror(errno));
      LOG(mq_logger, log_message);
    }
  }
}

void *sock_comm_heartbeat_notifier(void *args)
{
  mq_payload_heartbeat_t sock_comm_heartbeat;
  char log_message[128];

  bzero(&sock_comm_heartbeat, sizeof(sock_comm_heartbeat));
  sock_comm_heartbeat.sender_id = LOGGER_TASK_ID;
  sock_comm_heartbeat.heartbeat_status = true;

  while(1)
  {
    sem_wait(&sem_logger);

    if(send_heartbeat[LOGGER_TASK_ID])
    {
        if(mq_send(mq_heartbeat, (char *) &sock_comm_heartbeat, sizeof(sock_comm_heartbeat), 1) < 0)
        {
          bzero(log_message, sizeof(log_message));
          sprintf(log_message, "## SOCK COMM ## Unable to send heartbeat. %s", strerror(errno));
          LOG(mq_logger, log_message);
        }

        send_heartbeat[LOGGER_TASK_ID] = false;
    }
  }
}
