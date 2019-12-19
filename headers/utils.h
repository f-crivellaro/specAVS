/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  19-12-2019 14:13:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fernando Crivellaro (fc), fernando.crivellaro@compta.pt
 *        Company:  Compta SA, Portugal
 *
 * =====================================================================================
 */

#include <stdbool.h>

#define clear() printf("\033[H\033[J")

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "spectral"
#define TOPIC       "spec"
#define PAYLOAD     "boot"
#define QOS         1
#define TIMEOUT     10000L
#define INTERNAL    0
#define EXTERNAL    1

MQTTClient client;
MQTTClient_deliveryToken token;
MQTTClient_deliveryToken deliveredtoken;

MQTTClient outclient;
MQTTClient_deliveryToken outtoken;
MQTTClient_deliveryToken deliveredouttoken;

void handle_shutdown(int sig);
