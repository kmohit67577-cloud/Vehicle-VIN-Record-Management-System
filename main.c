#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "vehicle.h"

#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"

void displayHeader(void);
void displayMenu(void);
int getMenuChoice(void);
void handleMenuChoice(int choice);

int main(void)
{
    int choice;
    char confirmation[20];

    /* Initialize random number generator */
    srand((unsigned int)time(NULL));

    printf(CYAN BOLD);
    printf("\n============================================\n");
    printf("       VEHICLE DATABASE INITIALIZATION\n");
    printf("============================================\n");
    printf(RESET);

    /* Load existing database */
    if (!initializeVehicleDatabase())
    {
        printf(RED "\nDatabase loading failed!\n" RESET);
        printf("Application stopped to protect existing records.\n");
        return 1;
    }

    while (1)
    {
        displayHeader();
        displayMenu();

        choice = getMenuChoice();

        if (choice == -1)
        {
            printf("\nInput closed. Exiting application.\n");
            break;
        }

        if (choice == 9)
        {
            printf("\nAre you sure you want to exit? (Y/N): ");

            if (fgets(confirmation, sizeof(confirmation), stdin) == NULL)
                break;

            if (toupper((unsigned char)confirmation[0]) == 'Y')
            {
                printf(GREEN "\nThank you for using our system!\n" RESET);
                break;
            }

            printf(YELLOW "\nReturning to main menu...\n" RESET);
        }
        else
        {
            handleMenuChoice(choice);

            printf("\nPress Enter to continue...");

            if (getchar() == EOF)
                break;
        }
    }

    return 0;
}

/* Display application header */
void displayHeader(void)
{
    printf(CYAN BOLD);
    printf("\n============================================================\n");
    printf("          VEHICLE VIN RECORD MANAGEMENT SYSTEM              \n");
    printf("============================================================\n");
    printf(RESET);

    printf("                    VEHICLE DATABASE\n");
    printf("                       Version 1.0\n");
}

/* Display main menu */
void displayMenu(void)
{
    printf(YELLOW BOLD);
    printf("\n----------------------- MAIN DASHBOARD ---------------------\n");
    printf(RESET);

    printf("\n");
    printf("  [1] Register New Vehicle\n");
    printf("  [2] Display All Vehicles\n");
    printf("  [3] Search Vehicle\n");
    printf("  [4] Update Vehicle Information\n");
    printf("  [5] Delete Vehicle\n");
    printf("  [6] Sort Vehicle Records\n");
    printf("  [7] Vehicle Statistics\n");
    printf("  [8] Save Records\n");
    printf("  [9] Exit Application\n");

    printf("\n============================================================\n");
}

/* Get menu selection */
int getMenuChoice(void)
{
    char input[100];
    char *endPointer;
    long choice;

    while (1)
    {
        printf(CYAN "\nEnter your choice (1-9): " RESET);

        if (fgets(input, sizeof(input), stdin) == NULL)
            return -1;

        choice = strtol(input, &endPointer, 10);

        while (isspace((unsigned char)*endPointer))
            endPointer++;

        if (endPointer == input || *endPointer != '\0')
        {
            printf(RED "Invalid input! Enter a number.\n" RESET);
            continue;
        }

        if (choice < 1 || choice > 9)
        {
            printf(RED "Invalid choice! Select 1-9.\n" RESET);
            continue;
        }

        return (int)choice;
    }
}

/* Handle dashboard operations */
void handleMenuChoice(int choice)
{
    switch (choice)
    {
        case 1:
            addVehicle();
            break;

        case 2:
            displayVehicles();
            break;

        case 3:
            searchVehicles();
            break;

        case 4:
            updateVehicle();
            break;

        case 5:
            deleteVehicle();
            break;

        case 6:
            sortVehicles();
            break;

        case 7:
            displayVehicleStatistics();
            break;

        case 8:
            if (saveVehicleDatabase())
                printf("\nRecords saved successfully!\n");
            else
                printf("\nFailed to save records!\n");
            break;

        default:
            printf("Invalid selection.\n");
    }
}
