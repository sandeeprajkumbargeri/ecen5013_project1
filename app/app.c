#include "app.h"


int main(void)
{
  int app_sock = 0;
  char app_sockaddr_path[32], remote_sockaddr_path[32];
  unsigned char request;
  char command[16], response[64];
  struct sockaddr_un app_sockaddr, remote_sockaddr;
  socklen_t sockaddr_length;

  command_list_init();
  signal(SIGINT, signal_handler);

  app_sock = socket(AF_UNIX, SOCK_DGRAM, 0);     //create a datagram socket

  if(app_sock < 0)
    errExit("## ERROR ## Creating Socket: ");

  bzero(app_sockaddr_path, sizeof(app_sockaddr_path));
  sprintf(app_sockaddr_path, "/tmp/%u_socket", getpid());
  remove(app_sockaddr_path);

  bzero(remote_sockaddr_path, sizeof(remote_sockaddr_path));
  sprintf(remote_sockaddr_path, "/tmp/temp_light_sense_socket");

  sockaddr_length = sizeof(app_sockaddr);
  bzero(&app_sockaddr, sockaddr_length);
  bzero(&remote_sockaddr, sockaddr_length);

  app_sockaddr.sun_family = AF_UNIX;
  strncpy(app_sockaddr.sun_path, app_sockaddr_path, sizeof(app_sockaddr.sun_path) -1);

  remote_sockaddr.sun_family = AF_UNIX;
  strncpy(remote_sockaddr.sun_path, remote_sockaddr_path, sizeof(remote_sockaddr.sun_path) -1);

  if(bind(app_sock, (struct sockaddr *) &app_sockaddr, sizeof(struct sockaddr_un)) < 0)
    errExit("## ERROR ## Bind: ");

  printf("\n\t\t\t## AVAILABLE COMMANDS ##\nhelp     get [temp or light]     exit\n");

  while(1)
  {
    bzero(command, sizeof(command));
    printf("\n$ ");
    fgets(command, sizeof(command), stdin);

    command[strlen(command) - 1] = '\0';
    request = parse_command(command);

    if(request == COMMAND_INVALID)
    {
      printf("\n## ERROR ## Invalid command.\n");
      continue;
    }

    else if(request == COMMAND_EXIT)
    {
      remove(app_sockaddr_path);
      break;
    }

    else if(request == COMMAND_HELP)
      printf("\n\t\t\t## AVAILABLE COMMANDS ##\nhelp     get [temp or light]     exit\n");

    else
    {
      if(sendto(app_sock, (const void *) &request, sizeof(request), 0, (const struct sockaddr *) &remote_sockaddr, sockaddr_length) < 0)
        errExit("## ERROR ## Sending request to remote: ");

      bzero(response, sizeof(response));

      if(recvfrom(app_sock, (void *) response, sizeof(response), 0, (struct sockaddr *) &remote_sockaddr, &sockaddr_length) < 0)
        errExit("## ERROR ## Receiving response from remote: ");

      printf("-> %s\n", response);
    }
  }

  exit(EXIT_SUCCESS);
}


unsigned char parse_command(unsigned char* input)
{
		unsigned int i = 0;

		for(i = 0; i < TOTAL_COMMANDS; i++)
		{
			if(strcmp(input, command_list[i]) == 0)
				return (unsigned char) i;
		}

		return (COMMAND_INVALID);
}


void signal_handler(int signal)
{
  char app_sockaddr_path[32];
  sprintf(app_sockaddr_path, "/tmp/%u_socket", getpid());

  if(signal == SIGINT)
  {
    printf("\n## SIGINT ## Removing the socket file descriptor \"%s\".\n", app_sockaddr_path);
    remove(app_sockaddr_path);
    exit(EXIT_SUCCESS);
  }
}

void command_list_init(void)
{
  strcpy(command_list[COMMAND_HELP], "help");
  strcpy(command_list[COMMAND_TEMP_READ_TLOW], "temp read t-low");
  strcpy(command_list[COMMAND_TEMP_READ_THIGH], "temp read t-high");
  strcpy(command_list[COMMAND_TEMP_READ_DATA_REG], "temp read data");
  strcpy(command_list[COMMAND_TEMP_SET_SD_ON], "temp set shutdown on");
  strcpy(command_list[COMMAND_TEMP_SET_SD_OFF], "temp set shutdown off");
  strcpy(command_list[COMMAND_TEMP_READ_RESOLUTION], "temp read resolution");
  strcpy(command_list[COMMAND_TEMP_READ_FAULT_BITS], "temp read fault-bits");
  strcpy(command_list[COMMAND_TEMP_READ_EM], "temp read ext-mode");
  strcpy(command_list[COMMAND_TEMP_SET_EM_ON], "temp set ext-mode on");
  strcpy(command_list[COMMAND_TEMP_SET_EM_OFF], "temp set ext-mode off");

  strcpy(command_list[COMMAND_TEMP_SET_CONV_RATE_0], "temp set conv-rate 0.25hz");
  strcpy(command_list[COMMAND_TEMP_SET_CONV_RATE_1], "temp set conv-rate 1hz");
  strcpy(command_list[COMMAND_TEMP_SET_CONV_RATE_2], "temp set conv-rate 4hz");
  strcpy(command_list[COMMAND_TEMP_SET_CONV_RATE_3], "temp set conv-rate 8hz");
  strcpy(command_list[COMMAND_TEMP_READ_CONV_RATE], "temp read conv-rate");


  strcpy(command_list[COMMAND_LIGHT_SET_INTG_TIME_0], "light set intg-time 13.7ms");
  strcpy(command_list[COMMAND_LIGHT_SET_INTG_TIME_1], "light set intg-time 101ms");
  strcpy(command_list[COMMAND_LIGHT_SET_INTG_TIME_2], "light set intg-time 402ms");
  strcpy(command_list[COMMAND_LIGHT_READ_INTG_TIME], "light read intg-time");
  strcpy(command_list[COMMAND_LIGHT_READ_GAIN], "light read gain");
  strcpy(command_list[COMMAND_LIGHT_SET_GAIN_HIGH], "light set gain high");
  strcpy(command_list[COMMAND_LIGHT_SET_GAIN_LOW], "light set gain low");
  strcpy(command_list[COMMAND_LIGHT_SET_INT_ENABLE], "light set int enable");
  strcpy(command_list[COMMAND_LIGHT_SET_INT_DISABLE], "light set int disable");
  strcpy(command_list[COMMAND_LIGHT_READ_IDENTIFY_REG], "light read id");
  strcpy(command_list[COMMAND_LIGHT_READ_INT_TRSHLD_LOW], "light read trshld low");
  strcpy(command_list[COMMAND_LIGHT_READ_INT_TRSHLD_HIGH], "light read trshld low");
  strcpy(command_list[COMMAND_LIGHT_SET_INT_TRSHLD_LOW], "light set trshld low");
  strcpy(command_list[COMMAND_LIGHT_SET_INT_TRSHLD_HIGH], "light set trshld high");

  strcpy(command_list[COMMAND_EXIT], "exit");
}

//Function to print the error to STDOUT and exit.
void errExit(char *strError)
{
  char app_sockaddr_path[32];
  sprintf(app_sockaddr_path, "/tmp/%u_socket", getpid());
  perror(strError);

  remove(app_sockaddr_path);
  exit(EXIT_FAILURE);
}
