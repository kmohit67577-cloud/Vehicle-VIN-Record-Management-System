
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "search.h"


/* Read search input safely */
static int readSearchInput(char input[], int size)
{
    if (fgets(input, size, stdin) == NULL)
        return 0;

    size_t length = strlen(input);

    if (length > 0 && input[length - 1] == '\n')
    {
        input[length - 1] = '\0';
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

/* Compare strings without case sensitivity */
static int equalIgnoreCase(const char first[], const char second[])
{
    int i = 0;

    while (first[i] != '\0' && second[i] != '\0')
    {
        if (tolower((unsigned char)first[i]) !=
            tolower((unsigned char)second[i]))
            return 0;

        i++;
    }

    return first[i] == '\0' && second[i] == '\0';
}

/* Check whether text contains keyword */
static int containsIgnoreCase(const char text[], const char keyword[])
{
    int textLength = (int)strlen(text);
    int keywordLength = (int)strlen(keyword);

    if (keywordLength == 0)
        return 0;

    for (int i = 0; i <= textLength - keywordLength; i++)
    {
        int match = 1;

        for (int j = 0; j < keywordLength; j++)
        {
            if (tolower((unsigned char)text[i + j]) !=
                tolower((unsigned char)keyword[j]))
            {
                match = 0;
                break;
            }
        }

        if (match)
            return 1;
    }

    return 0;
}

/* Display vehicle information */
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

/* Display matching records */
static void displayResults(const Vehicle vehicles[], int count,
                           const char keyword[], int searchType)
{
    int found = 0;

    for (int i = 0; i < count; i++)
    {
        int match = 0;

        switch (searchType)
        {
            case 1:
                match = equalIgnoreCase(vehicles[i].vin, keyword);
                break;

            case 2:
                match = containsIgnoreCase(vehicles[i].owner, keyword);
                break;

            case 3:
                match = equalIgnoreCase(
                    vehicles[i].registration, keyword);
                break;

            case 4:
                match = containsIgnoreCase(vehicles[i].model, keyword);
                break;
        }

        if (match)
        {
            displayVehicle(&vehicles[i]);
            found++;
        }
    }

    if (found == 0)
        printf("\nNo matching vehicle records found!\n");
    else
        printf("\nTotal matching records: %d\n", found);
}

/* Vehicle search menu */
void searchMenu(const Vehicle vehicles[], int count)
{
    int choice;
    char keyword[100];
    char input[20];
    char *endPointer;
    long menuChoice;

    if (count == 0)
    {
        printf("\nNo vehicle records available for searching!\n");
        return;
    }

    while (1)
    {
        printf("\n");
        printf("================================================\n");
        printf("                  VEHICLE SEARCH\n");
        printf("================================================\n");

        printf("\n");
        printf("  [1] Search by VIN\n");
        printf("  [2] Search by Owner Name\n");
        printf("  [3] Search by Registration Number\n");
        printf("  [4] Search by Vehicle Model\n");
        printf("  [5] Back to Main Menu\n");

        printf("\n================================================\n");
        printf("Enter your choice: ");

        if (!readSearchInput(input, sizeof(input)))
            return;

        menuChoice = strtol(input, &endPointer, 10);

        while (isspace((unsigned char)*endPointer))
            endPointer++;

        if (endPointer == input || *endPointer != '\0' ||
            menuChoice < 1 || menuChoice > 5)
        {
            printf("\nInvalid choice! Select between 1 and 5.\n");
            continue;
        }

        choice = (int)menuChoice;

        if (choice == 5)
        {
            printf("\nReturning to main menu...\n");
            return;
        }

        printf("\nEnter search keyword: ");

        if (!readSearchInput(keyword, sizeof(keyword)))
            return;

        if (keyword[0] == '\0')
        {
            printf("\nSearch keyword cannot be empty!\n");
            continue;
        }

        printf("\nSearching vehicle records...\n");

        displayResults(vehicles, count, keyword, choice);
    }
}
