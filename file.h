
#ifndef FILE_H
#define FILE_H

#include "vehicle.h"

/* Save all vehicle records */
int saveRecords(const Vehicle vehicles[], int count);

/* Load vehicle records */
int loadRecords(Vehicle vehicles[], int *count);

#endif
