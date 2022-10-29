// Created by Yehor on 08.09.2021.
#include "FileUtility.h"

int getFileByteLength(FILE* file) {
    long curPos = ftell(file);
    fseek(file, 0L, SEEK_END);
    long length = ftell(file);
    fseek(file, curPos, SEEK_SET);
    return length;
}

int getNextDataIndex(FILE* file, int sizeofData) {
    return getFileByteLength(file) / sizeofData;
}

int indexEntityComparator(const void* e1, const void* e2) {
    IndexEntity* elem1 = (IndexEntity*) e1;
    IndexEntity* elem2 = (IndexEntity*) e2;
    return (elem1->key < elem2->key) ? -1 : (elem1->key > elem2->key) ? 1 : 0;
}

void writeToPosition(void* data, int pos, int dataSize, FILE* file) {
    fseek(file, pos * dataSize, SEEK_SET);
    fwrite(data, dataSize, 1, file);
}

void readOnPosition(void* dest, int pos, int dataSize, FILE* file) {
    fseek(file, pos * dataSize, SEEK_SET);
    fread(dest, dataSize, 1, file);
}

void sortIndexTableRam(IndexEntity* arr, int length) {
    qsort(arr, length, sizeof(IndexEntity), indexEntityComparator);
}

void sortIndexTable() {
    FILE* file;
    file = fopen(indexTablePath, "ab+");

    int numberOfElements = getFileByteLength(file) / (int) sizeof(IndexEntity);
    IndexEntity* indexEntityArray = malloc(numberOfElements * sizeof(IndexEntity));

    // read elements from file
    fread(indexEntityArray, sizeof(IndexEntity), numberOfElements, file);

    // sort elements
    qsort(indexEntityArray, numberOfElements,
          sizeof(IndexEntity), indexEntityComparator);

    fclose(file);

    // delete previous file
    remove(indexTablePath);

    // create new file and write the result
    file = fopen(indexTablePath, "wb");

    fwrite(indexEntityArray, sizeof(IndexEntity), numberOfElements, file);
    fclose(file);
}

int linearSearchHelper(IndexEntity* arr, const int maxLeft, const int maxRight, int mid, int pk) {
    int i = mid - 1;
    while (i >= maxLeft && arr[i].key == pk) {
        if (!arr[i].deleted)
            return arr[i].addressIndex;
        i--;
    }
    i = mid + 1;
    while (i <= maxRight && arr[i].key == pk) {
        if (!arr[i].deleted)
            return arr[i].addressIndex;
        i++;
    }
    return -1;
}

int findFileIndexByPkRam(IndexEntity* arr, int length, int pk, bool toDelete) {
    if (pk < 0)
        return - 1;

    int left = 0;
    int right = length - 1;

    while (left <= right) {
        int mid = (left + right) / 2;

        if (arr[mid].key == pk) {
            if (arr[mid].deleted)
                return linearSearchHelper(arr, 0, length - 1, mid, pk);

            if (toDelete)
                arr[mid].deleted = true;

            return arr[mid].addressIndex;
        }

        if (arr[mid].key < pk)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

ReviewMetaEntity findLastReviewInChain(CustomerMetaEntity* firstIndex, int* index) {
    FILE* file = fopen(reviewPath, "rb");
    int next = firstIndex->nextReviewIndex;
    int prev = -1;

    ReviewMetaEntity reviewMetaEntity;
    while (next >= 0) {
        readOnPosition(&reviewMetaEntity, next, sizeof(ReviewMetaEntity), file);
        prev = next;
        next = reviewMetaEntity.nextReviewIndex;
    }

    if (index != NULL)
        *index = prev;

    fclose(file);
    return reviewMetaEntity;
}

bool deleteAllReviewsChain(CustomerMetaEntity* firstIndex) {
    int next = firstIndex->nextReviewIndex;

    FILE* file;
    file = fopen(reviewPath, "r+b");

    ReviewMetaEntity reviewMetaEntity;
    while (next >= 0) {
        readOnPosition(&reviewMetaEntity, next, sizeof(ReviewMetaEntity), file);
        reviewMetaEntity.deleted = true;

        writeToPosition(&reviewMetaEntity, next, sizeof(ReviewMetaEntity), file);
        next = reviewMetaEntity.nextReviewIndex;
    }

    fclose(file);
    return false;
}

void copyReviewData(ReviewMetaEntity* source, ReviewMetaEntity* dest) {
    dest->nextReviewIndex = source->nextReviewIndex;
    dest->deleted = source->deleted;
    dest->pk = source->pk;
    dest->fk = source->fk;
    dest->review = source->review;
}

bool deleteReview(CustomerMetaEntity* firstIndex, int pkDelete)
{
    int next = firstIndex->nextReviewIndex;

    // record
    if (next < 0) return false;

    FILE* file = fopen(reviewPath, "r+b");

    ReviewMetaEntity prevEntity;
    ReviewMetaEntity curEntity;

    bool first = true;
    int prev = -1;

    while (next >= 0) {
        readOnPosition(&curEntity, next, sizeof(ReviewMetaEntity), file);

        if (curEntity.pk == pkDelete) {
            // next after current entity
            int nextNext = curEntity.nextReviewIndex;

            if (first) {
                if (nextNext == -1)
                    firstIndex->nextReviewIndex = -1;
                else
                    firstIndex->nextReviewIndex = nextNext;
            } else {
                if (nextNext == -1)
                    prevEntity.nextReviewIndex = -1;
                else
                    prevEntity.nextReviewIndex = nextNext;
                writeToPosition(&prevEntity, prev, sizeof(ReviewMetaEntity), file);
            }
            curEntity.deleted = true;
            writeToPosition(&curEntity, next, sizeof(ReviewMetaEntity), file);
            break;
        }

        copyReviewData(&curEntity, &prevEntity);
        first = false;
        prev = next;
        next = curEntity.nextReviewIndex;
    }

    fclose(file);
    return next >= 0;
}

bool customerIsDeleted(void* element) {
    CustomerMetaEntity* customerMetaEntity = (CustomerMetaEntity*) element;
    return customerMetaEntity->deleted;
}

bool reviewIsDeleted(void* element) {
    ReviewMetaEntity* reviewMetaEntity = (ReviewMetaEntity*) element;
    return reviewMetaEntity->deleted;
}

bool indexIsDeleted(void* element) {
    IndexEntity* indexEntity = (IndexEntity*) element;
    return indexEntity->deleted;
}

void rewriteWithoutDeletedElements(const char* fileName, int sizeofData, void* tmpBuffer, bool(*isDeletedFilter)(void* element)) {
    FILE* source = fopen(fileName, "rb");
    int elements = getNextDataIndex(source, sizeofData);

    char tmpPath[100];
    strcpy(tmpPath, absolutePath);
    strcat(tmpPath, "tmp.bin");

    FILE* destination = fopen(tmpPath, "w+b");

    int position = 0;
    for (int i = 0; i < elements; ++i) {
        readOnPosition(tmpBuffer, i, sizeofData, source);
        if (!isDeletedFilter(tmpBuffer)) {
            writeToPosition(tmpBuffer, position, sizeofData, destination);
            position++;
        }
    }

    fclose(source);
    fclose(destination);

    // delete the previous file
    remove(fileName);
    // rename the new file
    rename(tmpPath, fileName);
}

void rewriteIndexTable(IndexEntity* arr, int length) {
    FILE* file = fopen(indexTablePath, "w+b");

    int position = 0;
    for (int i = 0; i < length; ++i) {
        if (!arr[i].deleted) {
            writeToPosition(&arr[i], position, sizeof(IndexEntity), file);
            position++;
        }
    }

    fclose(file);
}