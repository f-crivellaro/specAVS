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

#include "mqtt_client.h"


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
	AvsHandle specHandle;
	int RequiredSize = 0;
	int NrDevices = 0;
	

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
		printf("\nMeas received!");
		spec_meas(AVS_GetHandleFromSerial(strpayload));
	} else if (!strcmp(topicName, "stop"))
	{
		printf("\nStopping Spec: %d\n", AVS_Done());
	} else if (!strcmp(topicName, "start"))
	{
		printf("\nAVS_Init: %d", AVS_Init(0));
		NrDevices = AVS_UpdateUSBDevices();
		printf("\nNumber of Connected Devices: %d\n", NrDevices);
		RequiredSize = NrDevices * sizeof(AvsIdentityType);	
		
		AvsIdentityType *devices = (AvsIdentityType *) calloc(RequiredSize, sizeof(AvsIdentityType));
		AVS_GetList(RequiredSize, &RequiredSize, (AvsIdentityType *)devices);
		printf("\nDevice List: %s\n", devices);
	
		specHandle = AVS_Activate((AvsIdentityType *)devices);
		printf("\nDevice Handle Activated: %d", specHandle);
		rc = spec_config(specHandle, &SpecDefaultConfig);
	
		if (rc)
		{
		        printf("\nERROR: Configuration Failed!");
		        return -1;
		}
	}

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}


void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}


