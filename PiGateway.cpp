#include <stdio.h>
#include "Gateway.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>

static int fdjs = 0;

void writeJs(char *message)
{
    char buf[200];
    memset(buf,200,0);
    strcpy(buf,message);
    buf[strlen(message)]='\0';
    write(fdjs, buf, strlen(message));
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

int openJSSocket()
{
    int fd;
    char buf[200];
    char *socket_path="/tmp/testjs.sock";
    struct sockaddr_un addr;
 
    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(-1);
  }
  printf("Opened %s\n",socket_path);

  return fd; 

}

int listenCSocket()
{
    struct sockaddr_un addr;
  int fd;
  char *socket_path="/tmp/testc.sock";


  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

  unlink(socket_path);

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  if (listen(fd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }

  // Fix permission so that the node.js app can write to this socket
  // TODO: set the same owner as for the node.js process instead
  chmod (socket_path, 0777);
  return fd;

}

int main(int argc, const char* argv[])
{
    fdjs = openJSSocket(); 

    int fd = listenCSocket();
    //char *test="Test\n"; 
    //write(fd2,test,6);
    writeJs("Test2\n");

    printf("Starting Gateway...\n");
    Gateway *gw = new Gateway("/dev/spidev0.0",8000000,25,3000);
    //Gateway *gw = new Gateway("/dev/spidev0.0",8000000,9,3000);
    printf("Gateway created...\n");
    if (gw == NULL)
    {
        printf("gw is null!");
    }
    gw->begin(0);
    printf("Begin called\n");
    while (1==1)
    {
        gw->processRadioMessage();
        if (inputAvailable(fd))
        {
            int cl;

            if ( (cl = accept(fd, NULL, NULL)) == -1) {
                perror("accept error");
                continue;
            }
            char buf[200];

            int nread = recv(cl,buf,200,0);

            gw->debug(PSTR("INP:%s.\n"),buf);
            gw->parseAndSend((String) buf);
        }
    }
    return 0;
}
