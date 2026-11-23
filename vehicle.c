#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "vehicle.h"
#include "vin.h"
#include "search.h"
#include "file.h"

/* Temporary vehicle database in RAM */
static Vehicle vehicles[MAX_VEHICLES];
static int vehicleCount = 0;

/* Initialize database when application starts */
int initializeVehicleDatabase(void)
{
    return loadRecords(vehicles, &vehicleCount);
}

/* Manually save database */
int saveVehicleDatabase(void)
{
    return saveRecords(vehicles, vehicleCount);
}

/* Read input safely */
static int readLine(char *buffer, int size)
{
    if (fgets(buffer, size, stdin) == NULL)
        return 0;

    size_t length = strlen(buffer);

    if (length > 0 && buffer[length - 1] == '\n')
    {
        buffer[length - 1] = '\0';
    }
    else
    {
        int ch;

        while ((ch = getchar()) != '\n' && ch != EOF)
        {
        }
    }

    return 1;
}

/* Get non-empty text input */
static int getTextInput(const char *message,
                        char *buffer, int size)
{
    while (1)
    {
        printf("%s", message);

        if (!readLine(buffer, size))
            return 0;

        if (buffer[0] == '\0')
        {
            printf("Input cannot be empty!\n");
            continue;
        }

        return 1;
    }
}

/* Convert registration to uppercase */
static void convertToUpper(char text[])
{
    for (int i = 0; text[i] != '\0'; i++)
    {
        text[i] = (char)toupper((unsigned char)text[i]);
    }
}

/* Check duplicate registration */
static int registrationExists(const char *registration,
                              int ignoredIndex)
{
    for (int i = 0; i < vehicleCount; i++)
    {
        if (i == ignoredIndex)
            continue;

        if (strcmp(vehicles[i].registration, registration) == 0)
            return 1;
    }

    return 0;
}

/* Get valid manufacturing year */
static int getValidYear(int *year)
{
    char input[30];
    char *endPointer;
    long value;

    while (1)
    {
        printf("Enter manufacturing year: ");

        if (!readLine(input, sizeof(input)))
            return 0;

        value = strtol(input, &endPointer, 10);

        while (isspace((unsigned char)*endPointer))
            endPointer++;

        if (endPointer != input &&
            *endPointer == '\0' &&
            value >= 1886 && value <= 2026)
        {
            *year = (int)value;
            return 1;
        }

        printf("Invalid year! Enter between 1886 and 2026.\n");
    }
}

/* Select vehicle type */
static int getVehicleType(char type[])
{
    char input[20];
    char *endPointer;
    long choice;

    printf("\nSelect vehicle type:\n");
    printf("1. Car\n");
    printf("2. Motorcycle\n");
    printf("3. SUV\n");
    printf("4. Other\n");

    while (1)
    {
        printf("Enter choice (1-4): ");

        if (!readLine(input, sizeof(input)))
            return 0;

        choice = strtol(input, &endPointer, 10);

        while (isspace((unsigned char)*endPointer))
            endPointer++;

        if (endPointer == input || *endPointer != '\0' ||
            choice < 1 || choice > 4)
        {
            printf("Invalid choice! Select 1-4.\n");
            continue;
        }

        switch (choice)
        {
            case 1:
                strcpy(type, "Car");
                break;

            case 2:
                strcpy(type, "Motorcycle");
                break;

            case 3:
                strcpy(type, "SUV");
                break;

            case 4:
                strcpy(type, "Other");
                break;
        }

        return 1;
    }
}

/* Display one vehicle */
static void displayVehicle(const Vehicle *vehicle)
{
    printf("\n------------------------------------------\n");
    printf("VIN          : %s\n", vehicle->vin);
    printf("Owner        : %s\n", vehicle->owner);
    printf("Model        : %s\n", vehicle->model);
    printf("Year         : %d\n", vehicle->year);
    printf("Type         : %s\n", vehicle->type);
    printf("Registration : %s\n", vehicle->registration);
    printf("------------------------------------------\n");
}

/* Find vehicle by VIN */
static int findVehicleByVIN(const char vin[])
{
    for (int i = 0; i < vehicleCount; i++)
    {
        if (strcmp(vehicles[i].vin, vin) == 0)
            return i;
    }

    return -1;
}

/* Register vehicle */
void addVehicle(void)
{
    Vehicle newVehicle;

    if (vehicleCount >= MAX_VEHICLES)
    {
        printf("\nVehicle database is full!\n");
        return;
    }

    printf("\n========== VEHICLE REGISTRATION ==========\n");

    if (!getTextInput("Enter owner name: ",
                      newVehicle.owner,
                      sizeof(newVehicle.owner)))
        return;

    if (!getTextInput("Enter vehicle model: ",
                      newVehicle.model,
                      sizeof(newVehicle.model)))
        return;

    if (!getValidYear(&newVehicle.year))
        return;

    if (!getVehicleType(newVehicle.type))
        return;

    while (1)
    {
        if (!getTextInput("Enter registration number: ",
                          newVehicle.registration,
                          sizeof(newVehicle.registration)))
            return;

        convertToUpper(newVehicle.registration);

        if (registrationExists(newVehicle.registration, -1))
        {
            printf("Registration number already exists!\n");
            continue;
        }

        break;
    }

    if (!generateUniqueVIN(vehicles, vehicleCount, newVehicle.vin))
    {
        printf("Unable to generate unique VIN!\n");
        return;
    }

    vehicles[vehicleCount] = newVehicle;
    vehicleCount++;

    printf("\nVehicle registered successfully!\n");
    displayVehicle(&newVehicle);

    if (!saveVehicleDatabase())
        printf("WARNING: Vehicle is currently saved only in memory!\n");
}

/* Display all vehicles */
void displayVehicles(void)
{
    if (vehicleCount == 0)
    {
        printf("\nNo vehicle records found!\n");
        return;
    }

    printf("\n========== REGISTERED VEHICLES ==========\n");

    for (int i = 0; i < vehicleCount; i++)
    {
        printf("\nVehicle Number: %d\n", i + 1);
        displayVehicle(&vehicles[i]);
    }

    printf("\nTotal registered vehicles: %d\n", vehicleCount);
}

/* Search connection */
void searchVehicles(void)
{
    searchMenu(vehicles, vehicleCount);
}

/* Update vehicle */
void updateVehicle(void)
{
    char vin[100];
    char confirmation[20];
    int index;

    printf("\n========== UPDATE VEHICLE ==========\n");

    if (vehicleCount == 0)
    {
        printf("No vehicle records available!\n");
        return;
    }

    if (!getTextInput("Enter VIN of vehicle to update: ",
                      vin, sizeof(vin)))
        return;

    index = findVehicleByVIN(vin);

    if (index == -1)
    {
        printf("\nVehicle not found!\n");
        return;
    }

    printf("\nExisting vehicle information:");
    displayVehicle(&vehicles[index]);

    printf("\nAre you sure you want to update this record? (Y/N): ");

    if (!readLine(confirmation, sizeof(confirmation)))
        return;

    if (toupper((unsigned char)confirmation[0]) != 'Y')
    {
        printf("Update cancelled.\n");
        return;
    }

    Vehicle updatedVehicle = vehicles[index];

    printf("\nEnter new vehicle information.\n");

    if (!getTextInput("Enter new owner name: ",
                      updatedVehicle.owner,
                      sizeof(updatedVehicle.owner)))
        return;

    if (!getTextInput("Enter new vehicle model: ",
                      updatedVehicle.model,
                      sizeof(updatedVehicle.model)))
        return;

    if (!getValidYear(&updatedVehicle.year))
        return;

    if (!getVehicleType(updatedVehicle.type))
        return;

    while (1)
    {
        if (!getTextInput("Enter new registration number: ",
                          updatedVehicle.registration,
                          sizeof(updatedVehicle.registration)))
            return;

        convertToUpper(updatedVehicle.registration);

        if (registrationExists(updatedVehicle.registration, index))
        {
            printf("Registration number already exists!\n");
            continue;
        }

        break;
    }

    vehicles[index] = updatedVehicle;

    printf("\nVehicle updated successfully!\n");
    displayVehicle(&vehicles[index]);

    if (!saveVehicleDatabase())
        printf("WARNING: Updated record is currently only in memory!\n");
}

/* Delete vehicle */
void deleteVehicle(void)
{
    char vin[100];
    char confirmation[20];
    int index;

    printf("\n========== DELETE VEHICLE ==========\n");

    if (vehicleCount == 0)
    {
        printf("No vehicle records available!\n");
        return;
    }

    if (!getTextInput("Enter VIN of vehicle to delete: ",
                      vin, sizeof(vin)))
        return;

    index = findVehicleByVIN(vin);

    if (index == -1)
    {
        printf("\nVehicle not found!\n");
        return;
    }

    printf("\nVehicle selected for deletion:");
    displayVehicle(&vehicles[index]);

    printf("\nAre you sure you want to delete this vehicle? (Y/N): ");

    if (!readLine(confirmation, sizeof(confirmation)))
        return;

    if (toupper((unsigned char)confirmation[0]) != 'Y')
    {
        printf("\nDeletion cancelled.\n");
        return;
    }

    for (int i = index; i < vehicleCount - 1; i++)
    {
        vehicles[i] = vehicles[i + 1];
    }

    vehicleCount--;

    printf("\nVehicle deleted successfully!\n");
    printf("Remaining vehicles: %d\n", vehicleCount);

    if (!saveVehicleDatabase())
        printf("WARNING: Deletion is currently only in memory!\n");
}

/* Compare strings without considering case */
static int compareTextIgnoreCase(const char *first,
                                 const char *second)
{
    while (*first != '\0' && *second != '\0')
    {
        int a = tolower((unsigned char)*first);
        int b = tolower((unsigned char)*second);

        if (a != b)
            return a - b;

        first++;
        second++;
    }

    return tolower((unsigned char)*first) -
           tolower((unsigned char)*second);
}

/* Sort by owner name */
static int compareByOwner(const void *a, const void *b)
{
    const Vehicle *first = (const Vehicle *)a;
    const Vehicle *second = (const Vehicle *)b;

    return compareTextIgnoreCase(first->owner, second->owner);
}

/* Sort by model */
static int compareByModel(const void *a, const void *b)
{
    const Vehicle *first = (const Vehicle *)a;
    const Vehicle *second = (const Vehicle *)b;

    return compareTextIgnoreCase(first->model, second->model);
}

/* Sort by year: oldest first */
static int compareByYearAscending(const void *a, const void *b)
{
    const Vehicle *first = (const Vehicle *)a;
    const Vehicle *second = (const Vehicle *)b;

    if (first->year < second->year)
        return -1;

    if (first->year > second->year)
        return 1;

    return 0;
}

/* Sort by year: newest first */
static int compareByYearDescending(const void *a, const void *b)
{
    return compareByYearAscending(b, a);
}

/* Sort by registration number */
static int compareByRegistration(const void *a, const void *b)
{
    const Vehicle *first = (const Vehicle *)a;
    const Vehicle *second = (const Vehicle *)b;

    return compareTextIgnoreCase(
        first->registration,
        second->registration
    );
}

/* Sort vehicle records */
void sortVehicles(void)
{
    char input[30];
    char *endPointer;
    long choice;

    int (*comparisonFunction)(const void *, const void *) = NULL;

    if (vehicleCount == 0)
    {
        printf("\nNo vehicle records available for sorting!\n");
        return;
    }

    printf("\n========== SORT VEHICLE RECORDS ==========\n");
    printf("1. Owner Name (A-Z)\n");
    printf("2. Vehicle Model (A-Z)\n");
    printf("3. Manufacturing Year (Oldest to Newest)\n");
    printf("4. Manufacturing Year (Newest to Oldest)\n");
    printf("5. Registration Number (A-Z)\n");

    while (1)
    {
        printf("\nEnter sorting choice (1-5): ");

        if (!readLine(input, sizeof(input)))
            return;

        choice = strtol(input, &endPointer, 10);

        while (isspace((unsigned char)*endPointer))
            endPointer++;

        if (endPointer == input || *endPointer != '\0' ||
            choice < 1 || choice > 5)
        {
            printf("Invalid choice! Select a number from 1 to 5.\n");
            continue;
        }

        break;
    }

    switch (choice)
    {
        case 1:
            comparisonFunction = compareByOwner;
            break;

        case 2:
            comparisonFunction = compareByModel;
            break;

        case 3:
            comparisonFunction = compareByYearAscending;
            break;

        case 4:
            comparisonFunction = compareByYearDescending;
            break;

        case 5:
            comparisonFunction = compareByRegistration;
            break;
    }

    qsort(
        vehicles,
        (size_t)vehicleCount,
        sizeof(Vehicle),
        comparisonFunction
    );

    printf("\nVehicle records sorted successfully!\n");

    displayVehicles();

    if (saveVehicleDatabase())
        printf("\nSorted order saved to the database.\n");
    else
        printf("\nWARNING: Sorted order is currently only in memory.\n");
}

/* Display vehicle statistics */
void displayVehicleStatistics(void)
{
    int carCount = 0;
    int motorcycleCount = 0;
    int suvCount = 0;
    int otherCount = 0;

    int oldestIndex = 0;
    int newestIndex = 0;

    long totalYears = 0;

    if (vehicleCount == 0)
    {
        printf("\nNo vehicle records available for statistics!\n");
        return;
    }

    for (int i = 0; i < vehicleCount; i++)
    {
        if (compareTextIgnoreCase(vehicles[i].type, "Car") == 0)
            carCount++;

        else if (compareTextIgnoreCase(
                     vehicles[i].type, "Motorcycle") == 0)
            motorcycleCount++;

        else if (compareTextIgnoreCase(
                     vehicles[i].type, "SUV") == 0)
            suvCount++;

        else
            otherCount++;

        totalYears += vehicles[i].year;

        if (vehicles[i].year < vehicles[oldestIndex].year)
            oldestIndex = i;

        if (vehicles[i].year > vehicles[newestIndex].year)
            newestIndex = i;
    }

    printf("\n========== VEHICLE STATISTICS ==========\n");

    printf("\nTotal registered vehicles : %d\n", vehicleCount);

    printf("\nVehicle type breakdown:\n");
    printf("Cars                       : %d\n", carCount);
    printf("Motorcycles                : %d\n", motorcycleCount);
    printf("SUVs                       : %d\n", suvCount);
    printf("Other                      : %d\n", otherCount);

    printf("\nOldest vehicle (by year):\n");
    displayVehicle(&vehicles[oldestIndex]);

    printf("\nNewest vehicle (by year):\n");
    displayVehicle(&vehicles[newestIndex]);

    printf("\nAverage manufacturing year : %.2f\n",
           (double)totalYears / vehicleCount);
}