#include "../include/sock_comm_task.h"


void *sock_comm_task_thread(void *args)
{
  int remote_sock = 0;
  char sockaddr_path[32];
  char response[64];
  struct sockaddr_un app_sockaddr, remote_sockaddr;
  socklen_t sockaddr_length;
  comm_payload_t request;

  remote_sock = socket(AF_UNIX, SOCK_DGRAM, 0);     //create a datagram socket

  if(remote_sock < 0)
    errExit("## ERROR ## Creating Socket: ");

  bzero(sockaddr_path, sizeof(sockaddr_path));
  sprintf(sockaddr_path, "/tmp/temp_light_sense_socket");
  remove(sockaddr_path);

  sockaddr_length = sizeof(remote_sockaddr);
  bzero(&app_sockaddr, sockaddr_length);
  bzero(&remote_sockaddr, sockaddr_length);

  remote_sockaddr.sun_family = AF_UNIX;
  strncpy(remote_sockaddr.sun_path, sockaddr_path, sizeof(remote_sockaddr.sun_path) -1);

  if(bind(remote_sock, (struct sockaddr *) &remote_sockaddr, sizeof(struct sockaddr_un)) < 0)
    errExit("## ERROR ## Bind: ");

  while(1)
  {
    bzero(&request, sizeof(request));

    if(recvfrom(remote_sock, (void *) &request, sizeof(request), 0, (struct sockaddr *) &app_sockaddr, &sockaddr_length) < 0)
      errExit("## ERROR ## Receiving request from app: ");

    if(request.command > 0x00 && request.command < 0x10)
    {
      if(mq_send(mq_temp, (const char *) &request, sizeof(comm_payload_t), 0) < 0)
        errExit("Error Sending Request to Temp Task");

      temp_asynch = true;
      sem_post(&sem_temp);
    }

    if(request.command > 0x0F && request.command < 0x1E)
    {
      if(mq_send(mq_light, (const char *) &request, sizeof(comm_payload_t), 0) < 0)
        errExit("Error Sending Request to Light Task");

      light_asynch = true;
      sem_post(&sem_light);
    }

    bzero(response, sizeof(response));

    if(mq_receive(mq_sock_comm, (char *) &response, sizeof(response), 0) < 0)
      errExit("Error receiving response");

    if(sendto(remote_sock, (const void *) response, sizeof(response), 0, (const struct sockaddr *) &app_sockaddr, sockaddr_length) < 0)
      errExit("## ERROR ## Sending response to app: ");
  }
}

//Function to print the error to STDOUT and exit.
void errExit(char *strError)
{
  perror(strError);
  exit(EXIT_FAILURE);
}


/*i2c_request_t parse_request(comm_payload_t request)
{
  i2c_request_t parsed;
  bzero(&parsed, sizeof(parsed));

  switch(request.command)
  {
    case COMMAND_TEMP_READ_TLOW:
        set_parsed_data(TEMP, READ, ADDRESS_TEMP_TLOW_REG, 0, 0);
        break;
    case COMMAND_TEMP_READ_THIGH:
      set_parsed_data(TEMP, READ, ADDRESS_TEMP_THIGH_REG, 0, 0);
      break;
    case COMMAND_TEMP_READ_DATA_REG:
      set_parsed_data(TEMP, READ, ADDRESS_TEMP_TEMP_REG, 0, 0);
      break;
    case COMMAND_TEMP_SET_SD_ON:
      set_parsed_data(TEMP, WRITE, ADDRESS_TEMP_CONF_REG, 0x0100, OR);
      break;
    case COMMAND_TEMP_SET_SD_OFF:
      set_parsed_data(TEMP, WRITE, ADDRESS_TEMP_CONF_REG, 0xFEFF, AND);
      break;
    case COMMAND_TEMP_READ_RESOLUTION:
      set_parsed_data(TEMP, WRITE, ADDRESS_TEMP_CONF_REG, 0xFEFF, AND);
      break;
    case COMMAND_TEMP_READ_FAULT_BITS:
    case COMMAND_TEMP_READ_EM:
    case COMMAND_TEMP_SET_EM_ON:
    case COMMAND_TEMP_SET_EM_OFF:

    case COMMAND_TEMP_SET_CONV_RATE_0:
    case COMMAND_TEMP_SET_CONV_RATE_1:
    case COMMAND_TEMP_SET_CONV_RATE_2:
    case COMMAND_TEMP_SET_CONV_RATE_3:
    case COMMAND_TEMP_READ_CONV_RATE:


    case COMMAND_LIGHT_SET_INTG_TIME_0:
    case COMMAND_LIGHT_SET_INTG_TIME_1:
    case COMMAND_LIGHT_SET_INTG_TIME_2:
    case COMMAND_LIGHT_READ_INTG_TIME:
    case COMMAND_LIGHT_READ_GAIN:
    case COMMAND_LIGHT_SET_GAIN_HIGH:
    case COMMAND_LIGHT_SET_GAIN_LOW:
    case COMMAND_LIGHT_SET_INT_ENABLE:
    case COMMAND_LIGHT_SET_INT_DISABLE:
    case COMMAND_LIGHT_READ_IDENTIFY_REG:
    case COMMAND_LIGHT_READ_INT_TRSHLD_LOW:
    case COMMAND_LIGHT_READ_INT_TRSHLD_HIGH:
    case COMMAND_LIGHT_SET_INT_TRSHLD_LOW:
    case COMMAND_LIGHT_SET_INT_TRSHLD_HIGH:
  }
}*/
