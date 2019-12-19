/*
 * =====================================================================================
 *
 *       Filename:  utils.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  19-12-2019 15:17:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fernando Crivellaro (fc), fernando.crivellaro@compta.pt
 *        Company:  Compta SA, Portugal
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include "MQTTClient.h"
#include "utils.h"

void handle_shutdown(int sig)
{

    printf("\nHandle Shutdown");
    MQTTClient_disconnect(client, 10000);
    MQTTClient_disconnect(outclient, 10000);

    printf("\nMQTT Clients Disconnected");
    MQTTClient_destroy(&client);
    printf("\nMQTT Clients Destroyed");

    printf("\nSpectral program closed\n");
    exit(0);
}

