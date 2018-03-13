#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <stdbool.h>

#define COMMAND_GET_TEMP_DATA   0x01
#define COMMAND_GET_LIGHT_DATA  0x02

void errExit(char *);


int main(void)
{
  int remote_sock = 0;
  char sockaddr_path[32];
  char request, response[64];
  struct sockaddr_un app_sockaddr, remote_sockaddr;
  socklen_t sockaddr_length;

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
    if(recvfrom(remote_sock, (void *) &request, sizeof(request), 0, (struct sockaddr *) &app_sockaddr, &sockaddr_length) < 0)
      errExit("## ERROR ## Receiving request from app: ");

    bzero(response, sizeof(response));

    if(request == COMMAND_GET_TEMP_DATA)
      strcpy(response, "Temperature = 30 C");
    else if(request == COMMAND_GET_LIGHT_DATA)
      strcpy(response, "Light = Turned ON");

    printf("Response = %s.\n", response);

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
