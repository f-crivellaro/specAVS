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
#include "log.h"

void handle_shutdown(int sig)
{

    log_info("EXIT: Handle Shutdown");
    MQTTClient_disconnect(client, 10000);

    log_info("EXIT: MQTT Clients Disconnected");
    MQTTClient_destroy(&client);
    log_info("EXIT: MQTT Clients Destroyed");

    log_info("EXIT: Spectral program closed");
    exit(0);
}

