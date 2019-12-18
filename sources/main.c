#include "main.h"

#define clear() printf("\033[H\033[J")

//volatile MQTTClient_deliveryToken deliveredtoken;

void myCleanupFun (void) __attribute__ ((destructor));

void myCleanupFun (void) 
{ 
	printf ("cleanup code after main()\n"); 
	//MQTTClient_disconnect(client, 10000);
	//MQTTClient_destroy(&client);

} 


void main()
{
	int NrDevices = 0;
	int RequiredSize = 0;
	AvsHandle specHandle;
	//int ch;
	//unsigned short npixels;
	//int poll = -1;
	int rc;
	
	
	// #############################################################################
	// Spectrometer Default Configuration
	// #############################################################################

	//MeasConfigType SpecDefaultConfig = {
    	//	0, 2047, 10, 0, 10, {0, 100}, {3,0}, 1, {0, 0, 0}, {0, 0, 0, 0, 0} 
	//};
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

	MQTTClient_create(&client, ADDRESS, CLIENTID,
	    MQTTCLIENT_PERSISTENCE_NONE, NULL);

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

	pubmsg.payload = PAYLOAD;
	pubmsg.payloadlen = strlen(PAYLOAD);
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	deliveredtoken = 0;

	MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);


	while(1);
}
