#ifndef VIN_H
#define VIN_H

#include "vehicle.h"

/* Generate a unique simulated VIN */
int generateUniqueVIN(Vehicle vehicles[], int count, char vin[]);

/* Validate VIN format */
int validateVIN(const char vin[]);

/* Check whether VIN already exists */
int isDuplicateVIN(const Vehicle vehicles[], int count,
                   const char vin[]);

#endif