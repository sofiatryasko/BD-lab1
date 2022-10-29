// Created by Yehor on 11.09.2021

#include "ReviewMetaEntity.h"

ReviewMetaEntity* newReviewMetaEntity(int pk, int fk, Review review) {
    struct ReviewMetaEntity* reviewMetaEntity = malloc(sizeof(ReviewMetaEntity));
    reviewMetaEntity->pk = pk;
    reviewMetaEntity->fk = fk;
    reviewMetaEntity->review = review;
    reviewMetaEntity->nextReviewIndex = -1;
    reviewMetaEntity->deleted = false;
    return reviewMetaEntity;
}

void printReview(ReviewMetaEntity* r) {
    printf("Review text: ");
    printf("%s\n", r->review.text);

    printf("Creation data: ");
    printf("%s\n", r->review.createdData);
}
