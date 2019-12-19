#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "avaspec.h"
#include "avs_driver.h"
#include "mqtt_client.h"
#include "utils.h"



void main()
{
	int NrDevices = 0;
	int RequiredSize = 0;
	AvsHandle specHandle;
	int rc;
	
    signal(SIGINT, handle_shutdown);

	
	// #############################################################################
	// Spectrometer Default Configuration
	// #############################################################################

	MeasConfigType MeasSpecConfig = SpecDefaultConfig;

	clear();
	printf("\nAVANTES Spectrometer Driver\n");
	printf("\nAVS_Init: %d",AVS_Init(0));

	NrDevices = AVS_UpdateUSBDevices();
	printf("\nNumber of Connected Devices: %d\n", NrDevices);
	RequiredSize = NrDevices * sizeof(AvsIdentityType);

	 // Configure a temporary buffer for the incoming data
	AvsIdentityType *devices = (AvsIdentityType *) calloc(RequiredSize, sizeof(AvsIdentityType));
	AVS_GetList(RequiredSize, &RequiredSize, (AvsIdentityType *)devices);
	printf("\nDevice List: %s\n", devices);
	
	specHandle = AVS_Activate((AvsIdentityType *)devices);
	printf("\nDevice Handle Activated: %d", specHandle);
	rc = spec_config(specHandle, &MeasSpecConfig);

	if (rc)
	{
		printf("\n ERROR: Configuration Failed, Finishing Program!");
		return;
	}


	// #############################################################################
	// MQTT Configuring
	// #############################################################################	
	//MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;

	//MQTTClient_deliveryToken token;
	int mqtt_rc;
	

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
	
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
	    printf("Failed to connect, return code %d\n", rc);
	    exit(EXIT_FAILURE);
	}

	MQTTClient_subscribe(client, "meas", QOS);
	MQTTClient_subscribe(client, "stop", QOS);
	MQTTClient_subscribe(client, "start", QOS);



    MQTTClient_create(&outclient, "tcp://10.11.0.26:1883", "outspectral", MQTTCLIENT_PERSISTENCE_NONE, NULL);
    
    MQTTClient_connectOptions conn_opts_out = MQTTClient_connectOptions_initializer;

    conn_opts_out.keepAliveInterval = 20;
    conn_opts_out.cleansession = 1;
    MQTTClient_setCallbacks(outclient, NULL, NULL, NULL, NULL);

    if ((rc = MQTTClient_connect(outclient, &conn_opts_out)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect to external broker, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    MQTTClient_subscribe(outclient, "id", QOS);


	pubmsg.payload = PAYLOAD;
	pubmsg.payloadlen = strlen(PAYLOAD);
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	deliveredtoken = 0;

	MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    MQTTClient_publishMessage(outclient, TOPIC, &pubmsg, &token);


	while(1);
}



