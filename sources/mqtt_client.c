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
    char* token;
    char* tokenIntegrationTime;
    char* tokenNrAverages;
    char* integrationTime;
    char* nrAverages;
    const char s[3] = ",";
    const char sEnd[3] = "}";
    const char sDouble[3] = ":";

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
	} else if (!strcmp(topicName, "config"))
	{
        tokenIntegrationTime = strtok(strpayload, s);       
        //printf("\n%s\n", tokenIntegrationTime);
        tokenNrAverages = strtok(0, sDouble);
        //printf("\n%s\n", tokenNrAverages);
        nrAverages = strtok(0, sEnd);
        SpecDefaultConfig.m_NrAverages = atoi(nrAverages);
        printf("\nNr. of Averages: %d\n", SpecDefaultConfig.m_NrAverages);    
        token = strtok(tokenIntegrationTime, ":");
        integrationTime = strtok(0, ":");
        SpecDefaultConfig.m_IntegrationTime = atof(integrationTime);
        printf("\nIntegration Time: %f\n", SpecDefaultConfig.m_IntegrationTime);
        spec_config(AVS_GetHandleFromSerial("0806052U1"), &SpecDefaultConfig);
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


