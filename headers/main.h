/*
 * =====================================================================================
 *
 *       Filename:  main.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05-12-2019 17:24:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dr. Fritz Mehner (mn), mehner@fh-swf.de
 *
 * =====================================================================================
 */

#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include "type.h"
#include "avaspec.h"
#include "avs_driver.h"
#include "mqtt_client.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "spectral"
#define TOPIC       "spec"
#define PAYLOAD     "boot"
#define QOS         1
#define TIMEOUT     10000L

MQTTClient client;
MQTTClient_deliveryToken token;
MQTTClient_deliveryToken deliveredtoken;

MeasConfigType SpecDefaultConfig = {
        0, 2047, 10, 0, 10, {0, 100}, {3,0}, 1, {0, 0, 0}, {0, 0, 0, 0, 0}
};


// Functions
void myCleanupFun (void);
//void measure_callback(AvsHandle* handle, int* new_scan);
//void write_spec_data(double wave, double data);


#endif
