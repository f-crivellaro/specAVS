/*
 * =====================================================================================
 *
 *       Filename:  avs_driver.c
 *
 *    Description:  Drivers of access to AVS Spectrometers
 *
 *        Version:  1.0
 *        Created:  05-12-2019 16:09:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fernando Crivellaro
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "type.h"
#include "avaspec.h"
#include "log.h"
#include "avs_driver.h"
#include "mqtt_client.h"
#include "utils.h"

typedef struct
{
    double data[2048];
    char serial[16];
} json_meas;

json_meas json_msg;


int spec_config(AvsHandle inSpecHandle, MeasConfigType* inMeasSpecConfig)
{
	int rc=1;
	printf("\nConfigurating Spectrometer ...\n");
	rc = AVS_PrepareMeasure(inSpecHandle, inMeasSpecConfig);
	if (rc==0)
	{
		LastMeasSpecConfig = *inMeasSpecConfig;
		printf("Configuration Ended\n");
	}
	else
		printf("Configuration Failed\n");
	
	return rc;
}


void measure_callback(AvsHandle* handle, int* new_scan)
{
        printf("\nMeasurement Callback:");
        printf("\n\tHandle: %d", *handle);
        printf("\n\tNew Scan: %d", *new_scan);
}


void write_spec_data(double wave, double data)
{
        FILE *fptr;
        fptr = fopen("./test.csv","a");
        if(fptr == NULL)
        {
                printf("Error!");
                exit(1);
        }
        fprintf(fptr,"%f,%f\n",wave,data);
        fclose(fptr);
}

int spec_meas(char* serialnr)
{
	int timestamp;
	unsigned short npixels;
	int poll = -1;

    AvsHandle specHandle = AVS_GetHandleFromSerial(serialnr);
	AVS_GetNumPixels(specHandle, &npixels);
	printf("\nN Pixels: %d", npixels);
	double *wavelengths = (double *) calloc(npixels, sizeof(double));
	AVS_GetLambda(specHandle, wavelengths);
	printf("\nStart Wavelength: %f", wavelengths[0]);
	printf("\nStop  Wavelength: %f", wavelengths[npixels-1]);

	AVS_MeasureCallback(specHandle, &measure_callback, 1);

	poll = AVS_PollScan(specHandle);
	if (poll<0)
	{
		printf("\nDevice Error: %d\n", poll);
		return poll;
	}

	printf("\nStarting Polling: %d\0", poll);
	while(poll!=1)
	{
	        poll = AVS_PollScan(specHandle);
	        printf("\nPoll: %d\0", poll);
        	sleep(1);
	};

	double *specMeasures = (double *) calloc(npixels, sizeof(double));

	printf("\nResults: %d\0", AVS_GetScopeData(specHandle, &timestamp, specMeasures));


    printf("\n");
    char str[26705];
    char linestr[13];
    char aux_str[50];

    strcpy(str, "{\"serial_number\": \"");
    strcat(str, serialnr);
    strcpy(aux_str, "\",\"wave_start\": ");
    strcat(str, aux_str);
    sprintf(aux_str, "%4.6f", wavelengths[0]);
    strcat(str, aux_str);
    strcpy(aux_str, ",\"wave_end\": ");
    strcat(str, aux_str);
    sprintf(aux_str, "%4.6f", wavelengths[npixels-1]);
    strcat(str, aux_str);
    strcpy(aux_str, ",\"data\": [");
    strcat(str, aux_str);
    
	for (int i = 0; i < npixels; i++)
	{
	        write_spec_data(wavelengths[i],specMeasures[i]);
            if (i == npixels-1)
                sprintf(linestr, "%05.06f", specMeasures[i]);
            else
                sprintf(linestr, "%05.06f,", specMeasures[i]);
            
            strcat(str, linestr);
	}

    strcpy(aux_str, "]}");
    strcat(str, aux_str);

    printf(str);

    char topic[16] = "spec/meas";
    publishmsg(EXTERNAL, topic, str);
	printf("\nMeasure Ended!\n");		

	return 0;
}


void spec_stop()
{
    printf("\nStopping Spec: %d\n", AVS_Done());
}


void spec_init()
{  
    int rc;
    int NrDevices = 0;
    int RequiredSize = 0;
    AvsHandle specHandle;

    printf("\nAVS_Init: %d", AVS_Init(0));
    NrDevices = AVS_UpdateUSBDevices();
    printf("\nNumber of Connected Devices: %d\n", NrDevices);
    RequiredSize = NrDevices * sizeof(AvsIdentityType);

    AvsIdentityType *devices = (AvsIdentityType *) calloc(RequiredSize, sizeof(AvsIdentityType));
    AVS_GetList(RequiredSize, &RequiredSize, (AvsIdentityType *)devices);
    printf("\nDevice List: %s\n", devices);

    specHandle = AVS_Activate((AvsIdentityType *)devices);
    printf("\nDevice Handle Activated: %d", specHandle);
    rc = spec_config(specHandle, &SpecDefaultConfig);

    if (rc)
    {
        printf("\nERROR: Configuration Failed!");
        //return -1;
    }
}
