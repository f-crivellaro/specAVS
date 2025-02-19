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



void main(int argc, char *argv[])
{
	int NrDevices = 0;
	int RequiredSize = 0;
	AvsHandle specHandle;
	int rc;

    log_set_level(atoi(argv[1]));
    fptr_log = fopen("/home/hermes/specAVS/log/spectral.log","w");
    if(fptr_log == NULL)
    {
        log_error("SYSTEM: Error opening log file!");
        exit(EXIT_FAILURE);
    }
    log_set_fp(fptr_log);

    log_info("#########################################");
    log_info("# Avantes Spectral Driver");
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
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;


    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

	int mqtt_trials = 0;

    for (int mqtt_trials = 0; mqtt_trials < 5; mqtt_trials++)
    {

	    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    	{
	        log_error("MQTT: Failed to connect to MQTT Broker, return code %d", rc);
	        log_info("MQTT: Trying reconnection...");
	        sleep(1);
    	} else
        {
            log_info("MQTT: Client Connected.");
            break;
        }
    }

	MQTTClient_subscribe(client, "spec/cmd/meas", QOS);
	MQTTClient_subscribe(client, "spec/cmd/stop", QOS);
	MQTTClient_subscribe(client, "spec/cmd/start", QOS);
	MQTTClient_subscribe(client, "spec/cmd/config", QOS);

    log_info("");
    log_info("SYSTEM: All Configuration Done!");
    log_info("");


	while(1);
}



