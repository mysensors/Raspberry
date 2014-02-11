#ifndef PiGateway_h
#define PiGateway_h
#define SERVERPORT 1315
#include "Gateway.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/stat.h>


void writeJs(char *message);

#endif
