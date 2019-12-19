/*
 * =====================================================================================
 *
 *       Filename:  mqtt_client.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05-12-2019 15:45:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fernando Crivellaro
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqtt_client.h"
#include "avs_driver.h"
#include "utils.h"


void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    //deliveredtoken = dt;
}


int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	int i, rc;
	char* payloadptr;
	char strpayload[512];

	printf("\n\nMessage arrived\n");
	printf("     topic: %s\n", topicName);
	printf("   message: ");
	payloadptr = message->payload;
	for(i=0; i<message->payloadlen; i++)
	{
		strpayload[i] = *payloadptr;
		putchar(*payloadptr++);
	}
	strpayload[message->payloadlen]='\0';
	putchar('\n');

	//printf("\nPayload Len: %d", message->payloadlen);
	//printf("\nPayload: %s\n", strpayload);

	if (!strcmp(topicName, "meas"))
	{
		printf("\nMeasure request received!");
		spec_meas(strpayload);
	} else if (!strcmp(topicName, "stop"))
	{
		spec_stop();
	} else if (!strcmp(topicName, "start"))
	{
        spec_init();
	}

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}


void publishmsg(int brokertype, char *topic, void *msg)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = msg;
    pubmsg.payloadlen = strlen(msg);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    if (brokertype == INTERNAL)
        MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    else
        MQTTClient_publishMessage(outclient, topic, &pubmsg, &token);
} 


void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}


