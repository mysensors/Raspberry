/*
 Simple Programm to get Sensors-Values from the MySensors Network and publish them on a MQTT Broker.

 Created  by Busa <busa@gmx.ch>, based on:
	the PiGateway from https://github.com/mysensors/Raspberry
	libmosquitto Example from http://ivyco.blogspot.ch

 More Information about the Used Librarys can be found on:
	http://wiringpi.com/
	http://mosquitto.org/
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
*/
 
#include <stdio.h>
#include "MyMQTT.h"
#include <RF24.h>
#include "mosquitto.h"
#include <wiringPi.h>

#define BROKER_HOSTNAME "localhost"
#define BROKER_PORT 1884
#define BROKER_KEEPALIVE 60

#define RF24IRQ_PIN 3

MyMQTT *gw;

/* libMosquito Functions */
static void die(const char *msg);
static bool set_callbacks(struct mosquitto *m);
int postMosquitto(struct mosquitto *m);
static struct mosquitto *init();
void mqttPublish();
char mosqId[30];
struct mosquitto *mosq = NULL;

/* MySensors CallBack Function */
void msgCallback(char *msg){
	
	printf("[CALLBACK]%s", msg);
	size_t payload_sz = 20;
	char payload[payload_sz];
	size_t payloadlen = 0;
	size_t topic_sz = MQTT_MAX_PACKET_SIZE;
	char topic[topic_sz];
	snprintf(topic, topic_sz, "%s",strsep(&msg, ":"));
	payloadlen = snprintf(payload, payload_sz, "%s", strsep(&msg, ":"));

        int res = mosquitto_publish(mosq, NULL,topic , payloadlen, payload, 0, false);
                if (res != MOSQ_ERR_SUCCESS) {
                printf("message not published - error code:%i\n", res);
        }

}

/* Radio Processing Actions */
void processRadio(void)
{
        gw->processRadioMessage();
}

/* Interrupt Function */
void isr(void)
{
	processRadio();
}

void setup(void)
{
	printf("Starting Gateway...\n");
	gw = new MyMQTT(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ, (uint8_t)30000);
	
	if (gw == NULL)
    	{
        	printf("gw is null!");
    	}
	gw->begin(RF24_PA_LEVEL_GW, RF24_CHANNEL, RF24_DATARATE, &msgCallback);

    	printf("Connecting to MQTT GW...\n");
	mosq = init();
	// this code needs to post to mosquitto, if can't connect stop right away
	if(!mosq) { die("can't connect\n"); }

	if (!set_callbacks(mosq)) { die("set_callbacks() failure\n"); }
	if(mosquitto_connect(mosq, BROKER_HOSTNAME, BROKER_PORT, BROKER_KEEPALIVE)){
		die("Unable to connect.\n");
	}

  	if (wiringPiSetup () < 0) {
      		die("Unable to setup wiringPi: %s\n");
  	}

  	if ( wiringPiISR (RF24IRQ_PIN, INT_EDGE_FALLING, &isr) < 0 ) {
      		die("Unable to setup ISR: %s\n");
  	}

}


int main(int argc, char** argv) 
{
	setup();
	while(1){
		sleep(10000);
	}
	return 0;
}


/* Fail with an error message. */
void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	if(message->payloadlen){
		printf("%s %p\n", message->topic, message->payload);
	}else{
		printf("%s (null)\n", message->topic);
	}
	fflush(stdout);
}

void on_connect(struct mosquitto *mosq, void *userdata, int result)
{
	if(!result){
		/* Subscribe to broker information topics on successful connect. */
		mosquitto_subscribe(mosq, NULL, "$SYS/#", 2);
	}else{
		fprintf(stderr, "Connect failed\n");
	}
}

void on_subscribe(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	int i;
	printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
	for(i=1; i<qos_count; i++){
		printf(", %d", granted_qos[i]);
	}
	printf("\n");
}

void on_log(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	/* Pring all log messages regardless of level. */
	printf("%s\n", str);
}

/* A message was successfully published. */
static void on_publish(struct mosquitto *m, void *udata, int m_id) {
	printf("published successfully\n");
}

static void die(const char *msg) {
	fprintf(stderr, "%s", msg);
	exit(1);
}

static struct mosquitto *init() {
	mosquitto_lib_init();
	snprintf(mosqId, 30, "client_%d", 1);
	mosq = mosquitto_new(mosqId, true, NULL);
	return mosq;
}

/* Register the callbacks that the mosquitto connection will use. */
static bool set_callbacks(struct mosquitto *m) {
	// Set the logging callback. This should be used if you want event logging information from the client library.
	mosquitto_log_callback_set(m, on_log);
	// Set the connect callback. This is called when the broker sends a CONNACK message in response to a connection.
	mosquitto_connect_callback_set(m, on_connect);
	// Set the message callback. This is called when a message is received from the broker.
	mosquitto_message_callback_set(m, on_message);
	// Set the subscribe callback. This is called when the broker responds to a subscription request.
	// remove this since I don't subscribe to any topic
	// mosquitto_subscribe_callback_set(m, on_subscribe);
	// Set the publish callback. This is called when a message initiated with mosquitto_publish has been sent to the broker successfully.
	mosquitto_publish_callback_set(m, on_publish);
	return true;
}
