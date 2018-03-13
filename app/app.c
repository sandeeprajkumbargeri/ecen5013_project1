#include "app.h"


int main(void)
{
  int app_sock = 0;
  char app_sockaddr_path[32], remote_sockaddr_path[32];
  unsigned char request;
  char command[16], response[64];
  struct sockaddr_un app_sockaddr, remote_sockaddr;
  socklen_t sockaddr_length;

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
		unsigned char* commands[TOTAL_COMMANDS];
		unsigned char* match;

		commands[0] = "help";
		commands[1] = "get temp";
		commands[2] = "get light";
		commands[3] = "exit";

		for(i = 0; i < TOTAL_COMMANDS; i++)
		{
			match = strstr(input, commands[i]);

			if(match == input)
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

//Function to print the error to STDOUT and exit.
void errExit(char *strError)
{
  perror(strError);
  exit(EXIT_FAILURE);
}
