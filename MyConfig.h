/*
 The MySensors library adds a new layer on top of the RF24 library.
 It handles radio network routing, relaying and ids.

 Created by Henrik Ekblad <henrik.ekblad@gmail.com>
 12/10/14 - Ported to Raspberry Pi by OUJABER Mohamed <m.oujaber@gmail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
*/

#ifndef MyConfig_h
#define MyConfig_h

/***
 * Configure Sensor Network
 */
#define RF24_CHANNEL	   76             //RF channel for the sensor net, 0-127
#define RF24_DATARATE 	   RF24_250KBPS   //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
#define RF24_PA_LEVEL 	   RF24_PA_MAX    //Sensor PA Level == RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBM, and RF24_PA_MAX=0dBm
#define RF24_PA_LEVEL_GW   RF24_PA_LOW  //Gateway PA Level, defaults to Sensor net PA Level.  Tune here if using an amplified nRF2401+ in your gateway.
#define BASE_RADIO_ID 	   ((uint64_t)0xA8A8E1FC00LL) // This is also act as base value for sensor nodeId addresses. Change this (or channel) if you have more than one sensor network.

// MySensors online examples defaults
#define DEFAULT_CE_PIN 9
#define DEFAULT_CS_PIN 10


/***
 * Enable/Disable debug logging
 */
// #define DEBUG


#ifdef __Raspberry_Pi 
	#define vsnprintf_P vsnprintf
	#define snprintf_P snprintf
	#define PSTR(x) (x)
	#define printf_P printf
	#define strlen_P strlen
#endif

#endif
