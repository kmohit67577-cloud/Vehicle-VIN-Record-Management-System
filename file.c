
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

#define DATABASE_FILE "vehicles.dat"
#define TEMP_FILE     "vehicles.tmp"
#define BACKUP_FILE   "vehicles.bak"
#define FILE_VERSION  1

/* Database file header */
typedef struct
{
    char magic[8];
    unsigned int version;
    unsigned int count;

} DatabaseHeader;

/* Save records to a temporary file and replace database */
int saveRecords(const Vehicle vehicles[], int count)
{
    FILE *file;
    DatabaseHeader header = {"VINDB001", FILE_VERSION, 0};
    unsigned int savedCount;
    int hadOldFile = 0;

    if (count < 0 || count > MAX_VEHICLES)
    {
        printf("Invalid vehicle record count!\n");
        return 0;
    }

    savedCount = (unsigned int)count;
    header.count = savedCount;

    /* Create temporary database */
    file = fopen(TEMP_FILE, "wb");

    if (file == NULL)
    {
        printf("Error: Cannot create temporary database!\n");
        return 0;
    }

    /* Write database header */
    if (fwrite(&header, sizeof(header), 1, file) != 1)
    {
        fclose(file);
        remove(TEMP_FILE);
        printf("Error writing database header!\n");
        return 0;
    }

    /* Write vehicle records */
    if (count > 0 &&
        fwrite(vehicles, sizeof(Vehicle), (size_t)count, file) !=
        (size_t)count)
    {
        fclose(file);
        remove(TEMP_FILE);
        printf("Error writing vehicle records!\n");
        return 0;
    }

    /* Check file closing */
    if (fclose(file) != 0)
    {
        remove(TEMP_FILE);
        printf("Error closing temporary database!\n");
        return 0;
    }

    /* Remove an older backup */
    remove(BACKUP_FILE);

    /* Check whether an existing database is present */
    file = fopen(DATABASE_FILE, "rb");

    if (file != NULL)
    {
        fclose(file);

        if (rename(DATABASE_FILE, BACKUP_FILE) != 0)
        {
            remove(TEMP_FILE);
            printf("Error creating database backup!\n");
            return 0;
        }

        hadOldFile = 1;
    }

    /* Install the new database */
    if (rename(TEMP_FILE, DATABASE_FILE) != 0)
    {
        printf("Error replacing database!\n");

        if (hadOldFile)
        {
            if (rename(BACKUP_FILE, DATABASE_FILE) != 0)
                printf("Warning: Database backup needs recovery.\n");
        }

        remove(TEMP_FILE);
        return 0;
    }

    /* New database is installed; old backup is no longer needed */
    if (hadOldFile)
        remove(BACKUP_FILE);

    printf("\nVehicle records saved successfully!\n");
    printf("Total records saved: %d\n", count);

    return 1;
}

/* Load records from database */
int loadRecords(Vehicle vehicles[], int *count)
{
    FILE *file;
    DatabaseHeader header;
    Vehicle temporaryVehicles[MAX_VEHICLES];

    long fileSize;
    long expectedSize;

    *count = 0;

    file = fopen(DATABASE_FILE, "rb");

    if (file == NULL)
    {
        printf("\nDatabase not found. Starting with an empty database.\n");
        return 1;
    }

    /* Read database header */
    if (fread(&header, sizeof(header), 1, file) != 1)
    {
        fclose(file);
        printf("Error: Database header is incomplete or corrupted!\n");
        return 0;
    }

    /* Validate database identity and version */
    if (memcmp(header.magic, "VINDB001", 8) != 0 ||
        header.version != FILE_VERSION ||
        header.count > MAX_VEHICLES)
    {
        fclose(file);
        printf("Error: Invalid or unsupported database file!\n");
        return 0;
    }

    /* Check expected database size */
    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        printf("Error checking database size!\n");
        return 0;
    }

    fileSize = ftell(file);

    expectedSize = (long)sizeof(DatabaseHeader) +
                   (long)header.count * (long)sizeof(Vehicle);

    if (fileSize < 0 || fileSize != expectedSize)
    {
        fclose(file);
        printf("Error: Database file is incomplete or corrupted!\n");
        return 0;
    }

    /* Return to the beginning of vehicle records */
    if (fseek(file, (long)sizeof(DatabaseHeader), SEEK_SET) != 0)
    {
        fclose(file);
        printf("Error reading database records!\n");
        return 0;
    }

    /* Read into temporary array first */
    if (header.count > 0 &&
        fread(temporaryVehicles, sizeof(Vehicle),
              header.count, file) != header.count)
    {
        fclose(file);
        printf("Error loading vehicle records!\n");
        return 0;
    }

    fclose(file);

    /* Validate loaded vehicle strings */
    for (unsigned int i = 0; i < header.count; i++)
    {
        if (memchr(temporaryVehicles[i].vin, '\0',
                   sizeof(temporaryVehicles[i].vin)) == NULL ||
            memchr(temporaryVehicles[i].owner, '\0',
                   sizeof(temporaryVehicles[i].owner)) == NULL ||
            memchr(temporaryVehicles[i].model, '\0',
                   sizeof(temporaryVehicles[i].model)) == NULL ||
            memchr(temporaryVehicles[i].type, '\0',
                   sizeof(temporaryVehicles[i].type)) == NULL ||
            memchr(temporaryVehicles[i].registration, '\0',
                   sizeof(temporaryVehicles[i].registration)) == NULL)
        {
            printf("Error: Invalid text data in database!\n");
            return 0;
        }

        if (strlen(temporaryVehicles[i].vin) != 17 ||
            temporaryVehicles[i].year < 1886 ||
            temporaryVehicles[i].year > 2026)
        {
            printf("Error: Invalid vehicle record in database!\n");
            return 0;
        }
    }

    /* Copy validated records into the application */
    memcpy(vehicles, temporaryVehicles,
           header.count * sizeof(Vehicle));

    *count = (int)header.count;

    printf("\nVehicle records loaded successfully!\n");
    printf("Total records loaded: %d\n", *count);

    return 1;
}
