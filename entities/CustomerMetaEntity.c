// Created by Yehor on 11.09.2021.

#include "CustomerMetaEntity.h"

CustomerMetaEntity* newCustomerMetaEntity(int pk, Customer customer) {
    struct CustomerMetaEntity* customerMetaEntity = malloc(sizeof(CustomerMetaEntity));
    customerMetaEntity->pk = pk;
    customerMetaEntity->customer = customer;
    customerMetaEntity->deleted = false;
    customerMetaEntity->nextReviewIndex = -1;
    customerMetaEntity->numOfReviews = 0;
    return customerMetaEntity;
}

void printCustomer(CustomerMetaEntity* c) {
    printf("First name: ");
    printf("%s\n", c->customer.firstName);

    printf("Last name: ");
    printf("%s\n", c->customer.lastName);

    printf("Address: ");
    printf("%s\n", c->customer.address);
}
