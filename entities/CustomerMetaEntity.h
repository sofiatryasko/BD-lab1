// Created by Yehor on 08.09.2021.
#ifndef DATABASELAB_CUSTOMERMETAENTITY_H
#define DATABASELAB_CUSTOMERMETAENTITY_H

#include <stdbool.h>
#include "Customer.h"

typedef struct CustomerMetaEntity {
    int pk;
    Customer customer;
    bool deleted;
    int nextReviewIndex;
    int numOfReviews;
} CustomerMetaEntity;

CustomerMetaEntity* newCustomerMetaEntity(int pk, Customer customer);

void printCustomer(CustomerMetaEntity* c);

#endif //DATABASELAB_CUSTOMERMETAENTITY_H
