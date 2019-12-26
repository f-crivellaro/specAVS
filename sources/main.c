#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "avaspec.h"
#include "avs_driver.h"
#include "mqtt_client.h"
#include "utils.h"
#include "log.h"



void main()
{
	int NrDevices = 0;
	int RequiredSize = 0;
	AvsHandle specHandle;
	int rc;
	
	//clear(); // Clear console window

    log_set_level(LOG_DEBUG);
    fptr_log = fopen("../log/spectral.log","w");
    if(fptr_log == NULL)
    {
        log_error("Error opening log file!");
        exit(EXIT_FAILURE);
    }
    log_set_fp(fptr_log);

    log_info("#########################################");
    log_info("# Avantes Spectral Driver is Alive");
    log_info("#########################################");
    log_info("");
	
    signal(SIGINT, handle_shutdown);


	// #############################################################################
	// Spectrometer Default Configuration
	// #############################################################################
    MeasSpecConfig = SpecDefaultConfig;
    spec_init();


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
	    log_error("Failed to connect, return code %d", rc);
	    exit(EXIT_FAILURE);
	}

	MQTTClient_subscribe(client, "meas", QOS);
	MQTTClient_subscribe(client, "stop", QOS);
	MQTTClient_subscribe(client, "start", QOS);
	MQTTClient_subscribe(client, "config", QOS);



    MQTTClient_create(&outclient, "tcp://10.11.0.26:1883", "outspectral", MQTTCLIENT_PERSISTENCE_NONE, NULL);
    
    MQTTClient_connectOptions conn_opts_out = MQTTClient_connectOptions_initializer;

    conn_opts_out.keepAliveInterval = 20;
    conn_opts_out.cleansession = 1;
    MQTTClient_setCallbacks(outclient, NULL, NULL, NULL, NULL);

    if ((rc = MQTTClient_connect(outclient, &conn_opts_out)) != MQTTCLIENT_SUCCESS)
    {
        log_error("Failed to connect to external broker, return code %d\n", rc);
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

    log_info("All Configuration Done!");    

	while(1);
}



