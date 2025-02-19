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
#include "log.h"



int spec_config(AvsHandle inSpecHandle, MeasConfigType* inMeasSpecConfig)
{
	int rc=1;

	log_info("Driver: Configurating Spectrometer ...");
    log_debug("Driver: High Definition ADC: %d", AVS_UseHighResAdc(inSpecHandle, true));
	rc = AVS_PrepareMeasure(inSpecHandle, inMeasSpecConfig);
	if (rc==0)
	{
		LastMeasSpecConfig = *inMeasSpecConfig;
		log_info("Driver: Configuration Successful");
	}
	else
		log_error("Driver: Configuration Failed");

	return rc;
}


void measure_callback(AvsHandle* handle, int* new_scan)
{
    int timestamp;

    log_info("==================================");
    log_info("Measurement Callback:");
    log_info("==================================");
    log_info("Driver: Status: %d", *new_scan);
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
    log_debug("Driver: Last Meas took %f seconds to be made.", time_taken);

    log_info("Driver: Integration Time (s): %6.3f", LastMeasSpecConfig.m_IntegrationTime);
    log_info("Driver: Number of Averages: %d", LastMeasSpecConfig.m_NrAverages);
	AVS_GetNumPixels(*handle, &npixels);
	log_info("Driver: N Pixels: %d", npixels);
	double *wavelengths = (double *) calloc(npixels, sizeof(double));
	AVS_GetLambda(*handle, wavelengths);
	log_info("Driver: Start Wavelength: %f", wavelengths[0]);
	log_info("Driver: Stop  Wavelength: %f", wavelengths[npixels-1]);


	double *specMeasures = (double *) calloc(npixels, sizeof(double));

	log_debug("Driver: Results: %d\0", AVS_GetScopeData(*handle, &timestamp, specMeasures));


    char str[53600];
    char linestr[13];
    char aux_str[50];

    strcpy(str, "{\"serial_number\": \"");
    strcat(str, oneSerial);
    strcpy(aux_str, "\",\"integration_time\": ");
    strcat(str, aux_str);
    sprintf(aux_str, "%6.03f", LastMeasSpecConfig.m_IntegrationTime);
    strcat(str, aux_str);
    strcpy(aux_str, ",\"nr_averages\": ");
    strcat(str, aux_str);
    sprintf(aux_str, "%6d", LastMeasSpecConfig.m_NrAverages);
    strcat(str, aux_str);
    strcpy(aux_str, ",\"wave_start\": ");
    strcat(str, aux_str);
    sprintf(aux_str, "%04.06f", wavelengths[0]);
    strcat(str, aux_str);
    strcpy(aux_str, ",\"wave_end\": ");
    strcat(str, aux_str);
    sprintf(aux_str, "%04.06f", wavelengths[npixels-1]);
    strcat(str, aux_str);
    strcpy(aux_str, ",\"n_pixels\": ");
    strcat(str, aux_str);
    sprintf(aux_str, "%04d", npixels);
    strcat(str, aux_str);
    strcpy(aux_str, ",\"data\": [");
    strcat(str, aux_str);

	for (int i = 0; i < npixels; i++)
	{
        if (i == npixels-1)
            sprintf(linestr, "%05.06f", specMeasures[i]);
        else
            sprintf(linestr, "%05.06f,", specMeasures[i]);

            strcat(str, linestr);
	}

    strcpy(aux_str, "]");
    strcat(str, aux_str);

    strcpy(aux_str, ",\"wavelengths\": [");
    strcat(str, aux_str);

	for (int i = 0; i < npixels; i++)
	{
        if (i == npixels-1)
            sprintf(linestr, "%04.06f", wavelengths[i]);
        else
            sprintf(linestr, "%04.06f,", wavelengths[i]);

            strcat(str, linestr);
	}


    strcpy(aux_str, "]}");
    strcat(str, aux_str);

    char topic[16] = "spec/reply/meas";
    publishmsg(topic, str);
	log_info("Driver: Measure Ended!");
    log_info("");

}



int spec_meas(char* serialnr)
{

    AvsHandle specHandle = AVS_GetHandleFromSerial(serialnr);
    oneSerial = serialnr;
    t = clock();
	AVS_MeasureCallback(specHandle, &measure_callback, 1);

	return 0;
}


void spec_stop()
{
    log_info("Driver: Stopping Spec: %d", AVS_Done());
}


void spec_init()
{
    int rc;
    int NrDevices = 0;
    int RequiredSize = 0;
    AvsHandle specHandle;
    char topic[20] = "spec/reply/start";
    char str[500];
    char aux_str[50];

    log_info("");
	log_debug("Driver: AVS_Init: %d",AVS_Init(0));

	NrDevices = AVS_UpdateUSBDevices();
	log_info("Driver: Number of Connected Devices: %d", NrDevices);
	RequiredSize = NrDevices * sizeof(AvsIdentityType);

	 // Configure a temporary buffer for the incoming data
	AvsIdentityType *devices = (AvsIdentityType *) calloc(RequiredSize, sizeof(AvsIdentityType));
	AVS_GetList(RequiredSize, &RequiredSize, (AvsIdentityType *)devices);
	log_info("Driver: Device List: %s", devices);

	specHandle = AVS_Activate((AvsIdentityType *)devices);
	log_info("Driver: Device Handle Activated: %d", specHandle);
	rc = spec_config(specHandle, &MeasSpecConfig);

	if (rc)
	{
		log_error("ERROR: Configuration Failed, Finishing Program!");
		exit(EXIT_FAILURE);
	}
    log_info("");

    log_debug("Buffers Done");

    strcpy(str, "{\"serial\": \"0806052U1\"");
    AVS_GetNumPixels(specHandle, &npixels);
    strcpy(aux_str, ",\"npixels\": ");
    strcat(str, aux_str);
    sprintf(aux_str, "%04d}", npixels);
    strcat(str, aux_str);
    publishmsg(topic, str);
    log_info("Start replied");

}


void write_spec_data(double wave, double data)
{
        FILE *fptr;
        fptr = fopen("./test.csv","a");
        if(fptr == NULL)
        {
                log_error("Driver: Error Writing Spec Data File!");
                exit(1);
        }
        fprintf(fptr,"%f,%f\n",wave,data);
        fclose(fptr);
}

