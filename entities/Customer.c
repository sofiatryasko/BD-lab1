// Created by Yehor on 11.09.2021

#include "Customer.h"

Customer* newCustomer(const char firstName[64], char lastName[64], char address[128]) {
    struct Customer* customer = malloc(sizeof(Customer));
    strcpy(customer->firstName, firstName);
    strcpy(customer->lastName, lastName);
    strcpy(customer->address, address);
    return customer;
};