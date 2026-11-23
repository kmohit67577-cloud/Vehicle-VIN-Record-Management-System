#ifndef VEHICLE_H
#define VEHICLE_H

#define MAX_VEHICLES 100

typedef struct
{
    char vin[18];
    char owner[50];
    char model[50];
    int year;
    char type[30];
    char registration[20];

} Vehicle;

void addVehicle(void);
void displayVehicles(void);
void searchVehicles(void);
void updateVehicle(void);
void deleteVehicle(void);

/* Stage 7 functions */
void sortVehicles(void);
void displayVehicleStatistics(void);

/* Database operations */
int initializeVehicleDatabase(void);
int saveVehicleDatabase(void);

#endif