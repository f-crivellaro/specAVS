/*
 * =====================================================================================
 *
 *       Filename:  mqtt_client.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10-12-2019 13:50:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fernando Crivellaro
 *
 * =====================================================================================
 */

#ifndef _MQTT_CLIENT_H
#define _MQTT_CLIENT_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include "avs_driver.h"
#include "avaspec.h"
#include "main.h"

void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);

#endif
