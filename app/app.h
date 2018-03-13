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

#define TOTAL_COMMANDS          4

#define COMMAND_HELP            0x00
#define COMMAND_GET_TEMP_DATA   0x01
#define COMMAND_GET_LIGHT_DATA  0x02

#define COMMAND_EXIT            0x03
#define COMMAND_INVALID         0xFF


void errExit(char *);
unsigned char parse_command(unsigned char* input);
