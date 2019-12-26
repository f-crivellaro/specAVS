/*
 * =====================================================================================
 *
 *       Filename:  avs_driver.h
 *
 *    Description:  Header od the drivers for access to AVS Spectrometers
 *
 *        Version:  1.0
 *        Created:  05-12-2019 16:42:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fernando Crivellaro
 *
 * =====================================================================================
 */

#ifndef _AVS_DRIVER_H
#define _AVS_DRIVER_H

//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include "type.h"
#include <time.h>
#include "avaspec.h"
//#include "log.h"
//#include "main.h"

MeasConfigType SpecDefaultConfig = {
        0, 2047, 1.05, 0, 10, {0, 100}, {3,0}, 1, {0, 0, 0}, {0, 0, 0, 0, 0}
};

MeasConfigType LastMeasSpecConfig;

clock_t t;
unsigned short npixels;
char* oneSerial;

int spec_config(AvsHandle inSpecHandle, MeasConfigType* inMeasSpecConfig);
void measure_callback(AvsHandle* handle, int* new_scan);
void write_spec_data(double wave, double data);
int spec_meas(char* serialnr);
void spec_stop();
void spec_init();

#endif
