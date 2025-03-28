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
#define _MQTT_CLIENT_H

#include <stdbool.h>
#include "MQTTClient.h"
#include <cjson/cJSON.h>


void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);
void publishmsg(char *topic, void *msg);

#endif
