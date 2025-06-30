#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100

typedef struct Passenger {
    char name[50];
    char dob[15];
    int age;
    char mobile[15];
    int seatNo;
    int ticketID;
    int busNo;
    struct Passenger *next;
} Passenger;

typedef struct Bus {
    char busName[50];
    int busNo;
    int totalSeats;
    char start[30];
    char end[30];
    char date[15];
    char departure[10];
    char arrival[10];
    float fare;
    int dep_minutes;
    int *seatMatrix;
    struct Bus *next;
} Bus;

Bus *busHead = NULL;
Passenger *passengerHead = NULL;

int timeToMinutes(const char *timeStr) {
    int hour, minute;
    sscanf(timeStr, "%d:%d", &hour, &minute);
    return hour * 60 + minute;
}

void sortBusesByTime() {
    if (!busHead || !busHead->next) return;
    int swapped;
    do {
        swapped = 0;
        Bus **ptr = &busHead;
        while ((*ptr)->next) {
            Bus *a = *ptr;
            Bus *b = a->next;
            if (a->dep_minutes > b->dep_minutes) {
                a->next = b->next;
                b->next = a;
                *ptr = b;
                swapped = 1;
            }
            ptr = &((*ptr)->next);
        }
    } while (swapped);
}

void saveBuses() {
    FILE *fp = fopen("buses.dat", "wb");
    for (Bus *b = busHead; b; b = b->next) {
        fwrite(b, sizeof(Bus), 1, fp);
        fwrite(b->seatMatrix, sizeof(int), b->totalSeats, fp);
    }
    fclose(fp);
}

void loadBuses() {
    FILE *fp = fopen("buses.dat", "rb");
    if (!fp) return;
    while (1) {
        Bus *b = (Bus *)malloc(sizeof(Bus));
        if (fread(b, sizeof(Bus), 1, fp) != 1) {
            free(b);
            break;
        }
        b->seatMatrix = (int *)malloc(sizeof(int) * b->totalSeats);
        fread(b->seatMatrix, sizeof(int), b->totalSeats, fp);
        b->next = busHead;
        busHead = b;
    }
    fclose(fp);
    sortBusesByTime();
}

void savePassengers() {
    FILE *fp = fopen("passengers.dat", "wb");
    FILE *txtFile = fopen("passengers.txt", "w");  // Overwrite to keep only active tickets
    if (!fp || !txtFile) {
        printf("Error saving passenger data!\n");
        return;
    }
    for (Passenger *p = passengerHead; p; p = p->next) {
        fwrite(p, sizeof(Passenger), 1, fp);
        fprintf(txtFile, "Ticket ID: %d\n", p->ticketID);
        fprintf(txtFile, "Name: %s\n", p->name);
        fprintf(txtFile, "DOB: %s\n", p->dob);
        fprintf(txtFile, "Age: %d\n", p->age);
        fprintf(txtFile, "Mobile: %s\n", p->mobile);
        fprintf(txtFile, "Seat No: %d\n", p->seatNo);
        fprintf(txtFile, "Bus No: %d\n", p->busNo);
        fprintf(txtFile, "------------------------\n");
    }
    fclose(fp);
    fclose(txtFile);
}

void loadPassengers() {
    FILE *fp = fopen("passengers.dat", "rb");
    if (!fp) return;
    while (1) {
        Passenger *p = (Passenger *)malloc(sizeof(Passenger));
        if (fread(p, sizeof(Passenger), 1, fp) != 1) {
            free(p);
            break;
        }
        p->next = passengerHead;
        passengerHead = p;
    }
    fclose(fp);
}

void addBus() {
    Bus *b = (Bus *)malloc(sizeof(Bus));
    if (!b) {
        printf("Memory allocation failed.\n");
        return;
    }
    printf("\n--- Add New Bus ---\n");
    printf("Enter Bus Name: ");
    scanf("%s", b->busName);
    printf("Enter Bus No: ");
    scanf("%d", &b->busNo);
    printf("Total Seats: ");
    scanf("%d", &b->totalSeats);
    printf("Starting Point: ");
    scanf("%s", b->start);
    printf("Ending Point: ");
    scanf("%s", b->end);
    printf("Departure Time (HH:MM): ");
    scanf("%s", b->departure);
    printf("Arrival Time (HH:MM): ");
    scanf("%s", b->arrival);
    printf("Fare: ");
    scanf("%f", &b->fare);
    printf("Date (DD-MM-YYYY): ");
    scanf("%s", b->date);
    b->dep_minutes = timeToMinutes(b->departure);
    b->seatMatrix = (int *)malloc(sizeof(int) * b->totalSeats);
    for (int i = 0; i < b->totalSeats; i++) b->seatMatrix[i] = 0;
    b->next = busHead;
    busHead = b;
    sortBusesByTime();
    saveBuses();
    printf("\nBus Added Successfully!\n");
}

void viewBuses() {
    Bus *b = busHead;
    printf("\n--- Available Buses ---\n");
    while (b) {
        printf("\nBus No: %d\nName: %s\nRoute: %s -> %s\nTime: %s to %s\nFare: %.2f\nDate: %s\n",
               b->busNo, b->busName, b->start, b->end,
               b->departure, b->arrival, b->fare, b->date);
        b = b->next;
    }
}

void displaySeats(int *seatMatrix, int total) {
    printf("\n--- Seat Matrix ---\n");
    for (int i = 0; i < total; i++) {
        printf("Seat %2d: %-9s", i + 1, seatMatrix[i] == 0 ? "Available" : "Booked");
        if ((i + 1) % 4 == 0) printf("\n");
    }
    if (total % 4 != 0) printf("\n");
}

void bookTicket() {
    char from[30], to[30], date[15];
    printf("\n--- Book Ticket ---\n");
    printf("Enter Starting Point: ");
    scanf("%s", from);
    printf("Enter Ending Point: ");
    scanf("%s", to);
    printf("Enter Date (DD-MM-YYYY): ");
    scanf("%s", date);

    Bus *b = busHead;
    int found = 0;
    while (b) {
        if (strcmp(b->start, from) == 0 && strcmp(b->end, to) == 0 && strcmp(b->date, date) == 0) {
            printf("%d. Bus No: %d | Name: %s | Departure: %s | Fare: %.2f\n",
                   found + 1, b->busNo, b->busName, b->departure, b->fare);
            found++;
        }
        b = b->next;
    }
    if (!found) {
        printf("\nNo buses found for the entered route and date.\n");
        return;
    }

    int selectedBusNo;
    printf("Enter Bus No: ");
    scanf("%d", &selectedBusNo);

    Bus *selectedBus = busHead;
    while (selectedBus && selectedBus->busNo != selectedBusNo) {
        selectedBus = selectedBus->next;
    }
    if (!selectedBus) {
        printf("Invalid Bus Number.\n");
        return;
    }

    displaySeats(selectedBus->seatMatrix, selectedBus->totalSeats);

    int n;
    printf("Enter number of seats to book: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        int seatNo;
        int seatBooked = 0;
        while (!seatBooked) {
            printf("Enter seat number %d (1-%d): ", i + 1, selectedBus->totalSeats);
            scanf("%d", &seatNo);

            if (seatNo < 1 || seatNo > selectedBus->totalSeats || selectedBus->seatMatrix[seatNo - 1] == 1) {
                printf("Invalid or already booked seat. Try again.\n");
                continue;
            }

            Passenger *p = (Passenger *)malloc(sizeof(Passenger));
            printf("Enter Name: "); scanf("%s", p->name);
            printf("DOB (DD-MM-YYYY): "); scanf("%s", p->dob);
            printf("Age: "); scanf("%d", &p->age);
            printf("Mobile: "); scanf("%s", p->mobile);

            p->seatNo = seatNo;
            p->ticketID = rand() % 10000 + 1;
            p->busNo = selectedBus->busNo;

            selectedBus->seatMatrix[seatNo - 1] = 1;

            p->next = passengerHead;
            passengerHead = p;

            printf("Ticket Booked! Ticket ID: %d\n", p->ticketID);
            seatBooked = 1;
        }
    }

    savePassengers();
    saveBuses();
}

void cancelTicket() {
    int id;
    printf("\n--- Cancel Ticket ---\n");
    printf("Enter Ticket ID: ");
    scanf("%d", &id);

    Passenger *curr = passengerHead, *prev = NULL;
    while (curr) {
        if (curr->ticketID == id) {
            Bus *b = busHead;
            while (b) {
                if (b->busNo == curr->busNo) {
                    b->seatMatrix[curr->seatNo - 1] = 0;
                    break;
                }
                b = b->next;
            }

            FILE *cancelFile = fopen("cancelled_passengers.txt", "a");
            if (cancelFile) {
                fprintf(cancelFile, "CANCELLED TICKET\n");
                fprintf(cancelFile, "Ticket ID: %d\n", curr->ticketID);
                fprintf(cancelFile, "Name: %s\n", curr->name);
                fprintf(cancelFile, "DOB: %s\n", curr->dob);
                fprintf(cancelFile, "Age: %d\n", curr->age);
                fprintf(cancelFile, "Mobile: %s\n", curr->mobile);
                fprintf(cancelFile, "Seat No: %d\n", curr->seatNo);
                fprintf(cancelFile, "Bus No: %d\n", curr->busNo);
                fprintf(cancelFile, "------------------------\n");
                fclose(cancelFile);
            }

            if (prev) prev->next = curr->next;
            else passengerHead = curr->next;

            free(curr);
            savePassengers();
            saveBuses();
            printf("Ticket cancelled successfully.\n");
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    printf("Ticket ID not found.\n");
}

void agentMenu() {
    int ch;
    do {
        printf("\n--- Agent Menu ---\n");
        printf("1. Add Bus\n2. View Buses\n0. Logout\nEnter Choice: ");
        scanf("%d", &ch);
        switch (ch) {
            case 1: addBus(); break;
            case 2: viewBuses(); break;
            case 0: return;
            default: printf("Invalid choice.\n"); break;
        }
    } while (ch != 0);
}

void passengerMenu() {
    int ch;
    do {
        printf("\n--- Passenger Menu ---\n");
        printf("1. Book Ticket\n2. Cancel Ticket\n0. Logout\nEnter Choice: ");
        scanf("%d", &ch);
        switch (ch) {
            case 1: bookTicket(); break;
            case 2: cancelTicket(); break;
            case 0: return;
            default: printf("Invalid choice.\n"); break;
        }
    } while (ch != 0);
}

void freeMemory() {
    while (busHead) {
        Bus *tmp = busHead;
        busHead = busHead->next;
        free(tmp->seatMatrix);
        free(tmp);
    }
    while (passengerHead) {
        Passenger *tmp = passengerHead;
        passengerHead = passengerHead->next;
        free(tmp);
    }
}

int main() {
    srand(time(NULL));
    loadBuses();
    loadPassengers();

    int type;
    printf("\n===== BUS RESERVATION SYSTEM =====\n");
    printf("1. Travel Agent\n2. Passenger\nEnter User Type: ");
    scanf("%d", &type);

    char username[20], password[20];
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    if (type == 1 && strcmp(username, "agent") == 0 && strcmp(password, "1234") == 0)
        agentMenu();
    else if (type == 2 && strcmp(username, "passenger") == 0 && strcmp(password, "1234") == 0)
        passengerMenu();
    else
        printf("\nInvalid credentials.\n");

    freeMemory();
    return 0;
}
