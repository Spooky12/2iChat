#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define TEST(v,mess) if (v<0){perror(mess);	exit(0);}

#define PORT 5000
#define BUFF_MAX 1024
typedef char T_Tab[][BUFF_MAX];
