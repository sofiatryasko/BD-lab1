// Created by Yehor on 08.09.2021.

#ifndef DATABASELAB_CUSTOMER_H
#define DATABASELAB_CUSTOMER_H

#include <malloc.h>
#include <string.h>
#include <stdio.h>

typedef struct Customer {
    char firstName[64];
    char lastName[64];
    char address[128];
} Customer;

Customer* newCustomer(const char firstName[64], char lastName[64], char address[128]);

#endif //DATABASELAB_CUSTOMER_H
