// Created by Yehor on 08.09.2021.

#ifndef DATABASELAB_REVIEWMETAENTITY_H
#define DATABASELAB_REVIEWMETAENTITY_H

#include "Review.h"

typedef struct ReviewMetaEntity {
    int pk;
    int fk;
    Review review;
    int nextReviewIndex;
    bool deleted;
} ReviewMetaEntity;

ReviewMetaEntity* newReviewMetaEntity(int pk, int fk, Review review);

void printReview(ReviewMetaEntity* r);

#endif //DATABASELAB_REVIEWMETAENTITY_H
