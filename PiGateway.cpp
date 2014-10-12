/*
 The MySensors library adds a new layer on top of the RF24 library.
 It handles radio network routing, relaying and ids.

 Created  by OUJABER Mohamed <m.oujaber@gmail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
*/
 
#include <stdio.h>
#include "MyGateway.h"
#include <RF24.h>

MyGateway *gw;

void msgCallback(char *msg){
	printf("[CALLBACK]%s", msg);

}

void setup(void)
{
	printf("Starting Gateway...\n");
	gw = new MyGateway(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ, 30000);
	
	if (gw == NULL)
    {
        printf("gw is null!");
    }
    gw->begin(RF24_PA_LEVEL_GW, RF24_CHANNEL, RF24_DATARATE, &msgCallback);
}

void loop(void)
{
 	gw->processRadioMessage();
}

int main(int argc, char** argv) 
{
	setup();
	while(1)
		loop();
	
	return 0;
}