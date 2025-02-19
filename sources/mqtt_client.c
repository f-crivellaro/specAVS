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
}


int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	int i, rc;
	char* payloadptr;
	char strpayload[512];
    // char* token;
    // char* tokenIntegrationTime;
    // char* tokenNrAverages;
    char* serial;
    double integrationTime;
    int nrAverages;
    int nrAggregation;
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
    log_info("MQTT: Message received - {Topic: %s, Message: %s}", topicName, strpayload);


	if (!strcmp(topicName, "spec/cmd/meas"))
	{
		log_info("MQTT: Measure request received!");
		spec_meas(strpayload);
	} else if (!strcmp(topicName, "spec/cmd/stop"))
	{
		spec_stop();
	} else if (!strcmp(topicName, "spec/cmd/start"))
	{
        spec_init();
	} else if (!strcmp(topicName, "spec/cmd/config"))
	{
        log_info("==================================");
        log_info("New Device Configuration:");
        log_info("==================================");
        cJSON *json = cJSON_Parse(strpayload);
        if (json == NULL) {
            log_error("MQTT: Error parsing JSON\n");
            return 1;
        }

        cJSON *tokenSerial = cJSON_GetObjectItem(json, "serial");
        cJSON *tokenIntegrationTime = cJSON_GetObjectItem(json, "integrationTime");
        cJSON *tokenNrAverages = cJSON_GetObjectItem(json, "nrAverages");
        cJSON *tokenAggregationSamples = cJSON_GetObjectItem(json, "aggregation_samples");

        serial = tokenSerial->valuestring;
        integrationTime = tokenIntegrationTime->valuedouble;
        nrAverages = tokenNrAverages->valueint;
        nrAggregation = tokenAggregationSamples->valueint;

        MeasSpecConfig.m_NrAverages = nrAverages; //atoi(nrAverages);
        log_info("MQTT: Nr. of Averages: %d", MeasSpecConfig.m_NrAverages);

        MeasSpecConfig.m_IntegrationTime = integrationTime; //atof(integrationTime);
        log_info("MQTT: Integration Time: %f", MeasSpecConfig.m_IntegrationTime);
        spec_config(AVS_GetHandleFromSerial(serial), &MeasSpecConfig);
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


