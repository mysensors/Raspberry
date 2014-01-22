#include <stdio.h>
#include "Gateway.h"
#include <unistd.h>
bool inputAvailable()  
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return (FD_ISSET(0, &fds));
}
int main(int argc, const char* argv[])
{
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
        if (inputAvailable())
        {
            std::string cmd;
            std::getline(std::cin, cmd);
            gw->debug(PSTR("INP:%s.\n"),cmd.c_str());
            gw->parseAndSend((String) cmd.c_str());
        }
    }
    return 0;
}
