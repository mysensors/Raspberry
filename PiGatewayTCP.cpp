/*
 * Communicates via RF24. Retrieves and send data from/to the sensor radios.
 * Writes/Reads commands to/from the file sockets /tmp/testc.sock resp. 
 * /tmp/testjs.sock 
 * The commands are of the form:
 *
 * radioId;childId;msgTypeId;sensorTypeId;sensorValue
 *
 * Note: This programs need to be dynamically linked with librf24/librf24.so.1
 *
 * Author: Johan Ekblad 2014
 * License: GNU Public License Version 2
 */
#include <stdio.h>
#include "PiGatewayTCP.h"
#include "Gateway.h"
#include <unistd.h>
#include <sys/socket.h> // sys/un.h
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h> // sys/stat.h, fcntl.h
#include <netinet/tcp.h>
#include <sys/stat.h>

static int fdjs = 0;
static boolean wok = false;

void writeJs(char *message)
{
    char buf[200];
    memset(buf,200,0);
    strcpy(buf,message);
    buf[strlen(message)]='\0';
    printf("Writing back message");
    if (wok)
    {
      write(fdjs, buf, strlen(message));
    }
    else
    {
      printf("Throttle message\n");
    }
}

bool inputAvailable(int fd)  
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  int r = select(fd+1, &fds, NULL, NULL, &tv);
  if (r < 0) 
  {
      fprintf(stderr,"Error checking socket");
      return 0;
  }
  return r;
}

int listenCSocket()
{
  struct sockaddr_in addr;
  int fd;

  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  int on=1;
  setsockopt(fd, SOL_SOCKET,  SO_REUSEADDR,
                   (char *)&on, sizeof(on));
  
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SERVERPORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  // Up to 5 clients
  if (listen(fd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }

  return fd;

}

int main(int argc, const char* argv[])
{
    int fd = listenCSocket();
    printf("Listening on 0.0.0.0:%d",SERVERPORT);

    printf("Starting Gateway...\n");
    Gateway *gw = new Gateway("/dev/spidev0.0",8000000,25,3000);
    printf("Gateway created...\n");
    if (gw == NULL)
    {
        printf("gw is null!");
    }
    gw->begin(0);
    printf("Begin called\n");

    // Wait until we have input (client should first send a ping)
    while (!inputAvailable(fd))
    {
        gw->processRadioMessage();
    }
    int cl;
    if ( (cl = accept(fd, NULL, NULL)) == -1) {
      perror("accept error");
    }
    wok=true;
    fdjs = cl;

    while (1==1)
    {
        gw->processRadioMessage();
        if (inputAvailable(cl))
        {
            printf("We have input\n");

            char buf[200];

            int nread = recv(cl,buf,200,0);
            // Assume cmd\n (not cmd1\ncmd2\n)
            printf("Nread=%d\n",nread);

            gw->debug(PSTR("INP:%s.\n"),buf);
            gw->parseAndSend((String) buf);
            printf("Sent to JS\n");
        }
    }
    return 0;
}
