
#include <stdlib.h>
#include <string.h>

#include "vin.h"

#define VIN_CHARACTERS "0123456789ABCDEFGHJKLMNPRSTUVWXYZ"
#define VIN_LENGTH 17

/* Validate VIN format */
int validateVIN(const char vin[])
{
    if (strlen(vin) != VIN_LENGTH)
    {
        return 0;
    }

    for (int i = 0; i < VIN_LENGTH; i++)
    {
        if (strchr(VIN_CHARACTERS, vin[i]) == NULL)
        {
            return 0;
        }
    }

    return 1;
}



/* Check duplicate VIN */
int isDuplicateVIN(const Vehicle vehicles[], int count,
                   const char vin[])
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(vehicles[i].vin, vin) == 0)
        {
            return 1;
        }
    }

    return 0;
}

/* Generate unique simulated VIN */
int generateUniqueVIN(Vehicle vehicles[], int count, char vin[])
{
    const char characters[] = VIN_CHARACTERS;
    int characterCount = (int)strlen(characters);

    for (int attempt = 0; attempt < 1000; attempt++)
    {
        for (int i = 0; i < VIN_LENGTH; i++)
        {
            int randomIndex = rand() % characterCount;
            vin[i] = characters[randomIndex];
        }

        vin[VIN_LENGTH] = '\0';

        if (validateVIN(vin) &&
            !isDuplicateVIN(vehicles, count, vin))
        {
            return 1;
        }
    }

    return 0;
}
