#include <stdio.h>
#include "Gateway.h"

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
      
    }
    return 0;
}
