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
#include "log.h"

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    log_debug("MQTT: Message with token value %d delivery confirmed", dt);
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


    payloadptr = message->payload;
	for(i=0; i<message->payloadlen; i++)
	{
		strpayload[i] = *payloadptr;
		putchar(*payloadptr++);
	}
	strpayload[message->payloadlen]='\0';
	
    log_info("");
    log_info("MQTT: Message arrived - {Topic: %s, Message: %s}", topicName, strpayload);


	if (!strcmp(topicName, "meas"))
	{
		log_info("MQTT: Measure request received!");
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
        MeasSpecConfig.m_NrAverages = atoi(nrAverages);
        log_info("MQTT: Nr. of Averages: %d", MeasSpecConfig.m_NrAverages);    
        token = strtok(tokenIntegrationTime, ":");
        integrationTime = strtok(0, ":");
        MeasSpecConfig.m_IntegrationTime = atof(integrationTime);
        log_info("MQTT: Integration Time: %f", MeasSpecConfig.m_IntegrationTime);
        spec_config(AVS_GetHandleFromSerial("0806052U1"), &MeasSpecConfig);
	}

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}


void publishmsg(char *topic, void *msg)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = msg;
    pubmsg.payloadlen = strlen(msg);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
} 


void connlost(void *context, char *cause)
{
    log_error("MQTT: Connection lost");
    log_error("MQTT:     cause: %s", cause);
}


