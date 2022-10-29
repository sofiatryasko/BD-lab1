// Created by Yehor on 08.09.2021

#ifndef DATABASELAB_FILEUTILITY_H
#define DATABASELAB_FILEUTILITY_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "CRUD.h"
#include "../entities/IndexEntity.h"
#include "../entities/CustomerMetaEntity.h"
#include "../entities/ReviewMetaEntity.h"

int getFileByteLength(FILE* file);
int getNextDataIndex(FILE* file, int sizeofData);

void sortIndexTableRam(IndexEntity* arr, int length);

// This function uses binary search to find index.
// This function implies that the "index-table.bin" file is sorted
int findFileIndexByPkRam(IndexEntity* arr, int length, int pk, bool toDelete);
int indexEntityComparator(const void* e1, const void* e2);

void writeToPosition(void* data, int pos, int dataSize, FILE* file);
void readOnPosition(void* dest, int pos, int dataSize, FILE* file);

void copyReviewData(ReviewMetaEntity* source, ReviewMetaEntity* dest);

ReviewMetaEntity findLastReviewInChain(CustomerMetaEntity* firstIndex, int* index);
bool deleteAllReviewsChain(CustomerMetaEntity* firstIndex);
bool deleteReview(CustomerMetaEntity* firstIndex, int pkDelete);

void rewriteWithoutDeletedElements(const char* fileName, int sizeofData, void* tmpBuffer, bool(*isDeletedFilter)(void* element));
void rewriteIndexTable(IndexEntity* arr, int length);

bool customerIsDeleted(void* element);
bool reviewIsDeleted(void* element);
bool indexIsDeleted(void* element);

#endif //DATABASELAB_FILEUTILITY_H
