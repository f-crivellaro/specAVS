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

//#include "avaspec.h"
#include "avs_driver.h"

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

int spec_meas(AvsHandle inSpecHandle)
{
	int timestamp;
	unsigned short npixels;
	int poll = -1;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

	AVS_GetNumPixels(inSpecHandle, &npixels);
	printf("\nN Pixels: %d", npixels);
	double *wavelengths = (double *) calloc(npixels, sizeof(double));
	AVS_GetLambda(inSpecHandle, wavelengths);
	printf("\nStart Wavelength: %f", wavelengths[0]);
	printf("\nStop  Wavelength: %f", wavelengths[npixels-1]);

	AVS_MeasureCallback(inSpecHandle, &measure_callback, 1);

	poll = AVS_PollScan(inSpecHandle);
	if (poll<0)
	{
		printf("\nDevice Error: %d\n", poll);
		return poll;
	}

	printf("\nStarting Polling: %d\0", poll);
	while(poll!=1)
	{
	        poll = AVS_PollScan(inSpecHandle);
	        printf("\nPoll: %d\0", poll);
        	sleep(1);
	};

	double *specMeasures = (double *) calloc(npixels, sizeof(double));

	printf("\nResults: %d\0", AVS_GetScopeData(inSpecHandle, &timestamp, specMeasures));
	
	//for (int i = 0; i < npixels; i++)
	//{
	//        write_spec_data(wavelengths[i],specMeasures[i]);
	//}

	pubmsg.payload = specMeasures;
	pubmsg.payloadlen = npixels;
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	deliveredtoken = 0;

	MQTTClient_publishMessage(client, "spec/meas", &pubmsg, &token);

	printf("\nMeasure Ended!\n");		

	return 0;
}
