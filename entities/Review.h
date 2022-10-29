// Created by Yehor on 08.09.2021

#ifndef DATABASELAB_REVIEW_H
#define DATABASELAB_REVIEW_H

#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct Review {
    char text[512];

    // dd-mm-yyyy -> 10 bytes + '\0' = 11
    char createdData[11];
} Review;

Review* newReview(char text[512], char createdData[11]);

#endif //DATABASELAB_REVIEW_H
