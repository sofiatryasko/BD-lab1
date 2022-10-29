// Created by Yehor on 08.09.2021

#include "CRUD.h"

const char customerPath[] = "C:\\Users\\Yehor\\CLionProjects\\databaseLab\\tables\\customer.bin";
const char reviewPath[] = "C:\\Users\\Yehor\\CLionProjects\\databaseLab\\tables\\review.bin";
const char indexTablePath[] = "C:\\Users\\Yehor\\CLionProjects\\databaseLab\\tables\\index-table.bin";
const char absolutePath[] = "C:\\Users\\Yehor\\CLionProjects\\databaseLab\\tables\\";

IndexEntity* indexEntityArray;
int numberOfIndexEntityElements;

int masterNumDeleted;
int slaveNumDeleted;

void addIndexEntity(IndexEntity newEntity) {
    IndexEntity* tmpArr = malloc((numberOfIndexEntityElements + 1) * sizeof(IndexEntity));
    for (int i = 0; i < numberOfIndexEntityElements; ++i)
        tmpArr[i] = indexEntityArray[i];
    tmpArr[numberOfIndexEntityElements] = newEntity;

    free(indexEntityArray);
    indexEntityArray = tmpArr;
    numberOfIndexEntityElements++;
}

void runDataBase() {
    FILE* file = fopen(indexTablePath, "rb");
    numberOfIndexEntityElements = getFileByteLength(file) / (int) sizeof(IndexEntity);
    indexEntityArray = malloc(numberOfIndexEntityElements * sizeof(IndexEntity));

    // read elements from file
    fread(indexEntityArray, sizeof(IndexEntity), numberOfIndexEntityElements, file);
    fclose(file);

    sortIndexTableRam(indexEntityArray, numberOfIndexEntityElements);

    getNumDeleted(&masterNumDeleted, &slaveNumDeleted);
}

void stopDataBase() {
    setNumDeleted(masterNumDeleted, slaveNumDeleted);
    rewriteIndexTable(indexEntityArray, numberOfIndexEntityElements);

    // delete an array
    free(indexEntityArray);
}

bool createM(CustomerMetaEntity* data) {
    if (data->pk < 0)
        return false;

    CustomerMetaEntity tmp;
    // if the record already exists, update it
    if (readM(&tmp, data->pk, NULL, false) && !tmp.deleted)
        return updateM(data->pk, data);

    FILE* file = fopen(customerPath, "ab+");

    int dataIndex = getNextDataIndex(file, sizeof(CustomerMetaEntity));

    fwrite(data, sizeof(CustomerMetaEntity), 1, file);
    fclose(file);

    IndexEntity indexEntity;
    indexEntity.key = data->pk;
    indexEntity.addressIndex = dataIndex;
    indexEntity.deleted = false;

    addIndexEntity(indexEntity);

    fclose(file);
    return true;
}

bool createS(ReviewMetaEntity* data) {
    if (data->pk < 0)
        return false;

    ReviewMetaEntity tmp;
    // if the record already exists, update it
    if (readS(&tmp, data->pk, NULL) && !tmp.deleted)
        return updateS(data->pk, data);

    CustomerMetaEntity customerMetaEntity;
    int customerIndex;
    bool successfullyRead = readM(&customerMetaEntity, data->fk, &customerIndex, false);

    if (!successfullyRead)
        return false;

    FILE* file = fopen(reviewPath, "ab+");
    int dataIndex = getNextDataIndex(file, sizeof(ReviewMetaEntity));

    // means that there is no review records in the table
    if (customerMetaEntity.nextReviewIndex == -1)
        customerMetaEntity.nextReviewIndex = dataIndex;
    else {
        int lastReviewIndex;
        ReviewMetaEntity lastReview = findLastReviewInChain(&customerMetaEntity, &lastReviewIndex);
        lastReview.nextReviewIndex = dataIndex;

        FILE* reviewFile = fopen(reviewPath, "r+b");
        writeToPosition(&lastReview, lastReviewIndex, sizeof(ReviewMetaEntity), reviewFile);
        fclose(reviewFile);
    }

    FILE* customerFile = fopen(customerPath, "r+b");

    customerMetaEntity.numOfReviews++;
    writeToPosition(&customerMetaEntity, customerIndex, sizeof(CustomerMetaEntity), customerFile);

    fclose(customerFile);

    fwrite(data, sizeof(ReviewMetaEntity), 1, file);

    fclose(file);
    return true;
}

bool readM(CustomerMetaEntity* dest, int pk, int* index, bool toDelete) {
    sortIndexTableRam(indexEntityArray, numberOfIndexEntityElements);

    int recordIndex = findFileIndexByPkRam(indexEntityArray, numberOfIndexEntityElements, pk, toDelete);

    if (recordIndex == -1)
        return false;

    CustomerMetaEntity customerMetaEntity;
    FILE* file = fopen(customerPath, "rb");

    readOnPosition(&customerMetaEntity, recordIndex, sizeof(CustomerMetaEntity), file);
    fclose(file);

    if (index != NULL)
        *index = recordIndex;

    dest->pk = customerMetaEntity.pk;
    dest->deleted = customerMetaEntity.deleted;
    dest->nextReviewIndex = customerMetaEntity.nextReviewIndex;
    dest->customer = customerMetaEntity.customer;
    dest->numOfReviews = customerMetaEntity.numOfReviews;

    return true;
}

bool readS(ReviewMetaEntity* dest, int pk, int* index) {
    FILE* file = fopen(reviewPath, "rb");
    int fileLength = getNextDataIndex(file, sizeof(ReviewMetaEntity));

    ReviewMetaEntity reviewMetaEntity;
    int curIndex = 0;

    while (curIndex < fileLength) {
        readOnPosition(&reviewMetaEntity, curIndex, sizeof(ReviewMetaEntity), file);

        if (reviewMetaEntity.pk == pk && !reviewMetaEntity.deleted)
            break;

        curIndex++;
    }

    // "Review meta entity" wasn't found
    if (curIndex == fileLength) {
        fclose(file);
        return false;
    }

    if (index != NULL)
        *index = curIndex;

    copyReviewData(&reviewMetaEntity, dest);

    fclose(file);
    return true;
}

bool updateM(int pk, CustomerMetaEntity* updateData) {
    CustomerMetaEntity customerMetaEntity;
    int dataIndex;
    bool successfullyRead = readM(&customerMetaEntity, pk, &dataIndex, false);

    if (!successfullyRead || customerMetaEntity.pk != updateData->pk)
        return false;

    FILE* file;
    file = fopen(customerPath, "r+b");

    writeToPosition(updateData, dataIndex, sizeof(CustomerMetaEntity), file);

    fclose(file);
    return true;
}

bool updateS(int pk, ReviewMetaEntity* updateData) {
    ReviewMetaEntity reviewMetaEntity;
    int dataIndex;
    bool successfullyRead = readS(&reviewMetaEntity, pk, &dataIndex);

    if (!successfullyRead || reviewMetaEntity.pk != updateData->pk) {
        return false;
    }

    updateData->nextReviewIndex = reviewMetaEntity.nextReviewIndex;

    FILE* file;
    file = fopen(reviewPath, "r+b");

    writeToPosition(updateData, dataIndex, sizeof(ReviewMetaEntity), file);

    fclose(file);
    return true;
}

bool deleteM(int pk) {
    CustomerMetaEntity customerMetaEntity;
    int dataIndex;

    // IndexEntity will be marked as deleted in the "readM" function
    bool successfullyRead = readM(&customerMetaEntity, pk, &dataIndex, true);

    if (!successfullyRead)
        return false;

    deleteAllReviewsChain(&customerMetaEntity);

    FILE* file;
    file = fopen(customerPath, "r+b");

    customerMetaEntity.deleted = true;
    writeToPosition(&customerMetaEntity, dataIndex, sizeof(CustomerMetaEntity), file);

    masterNumDeleted++;
    fclose(file);
    return true;
}

bool deleteS(int pk) {
    ReviewMetaEntity reviewMetaEntity;
    bool successfullyRead = readS(&reviewMetaEntity, pk, NULL);

    if (!successfullyRead)
        return false;

    CustomerMetaEntity customerMetaEntity;
    int customerIndex;
    readM(&customerMetaEntity, reviewMetaEntity.fk, &customerIndex, false);

    bool successfullyDeleted = deleteReview(&customerMetaEntity, reviewMetaEntity.pk);

    if (!successfullyDeleted)
        return false;

    FILE* file = fopen(customerPath, "r+b");
    customerMetaEntity.numOfReviews--;
    writeToPosition(&customerMetaEntity, customerIndex, sizeof(CustomerMetaEntity), file);

    slaveNumDeleted++;
    fclose(file);
    return true;
}

int countM() {
    FILE* file = fopen(customerPath, "rb");
    int count = getNextDataIndex(file, sizeof(CustomerMetaEntity)) - masterNumDeleted;
    fclose(file);
    return count;
}

int countS() {
    FILE* file = fopen(reviewPath, "rb");
    int count = getNextDataIndex(file, sizeof(ReviewMetaEntity)) - slaveNumDeleted;
    fclose(file);
    return count;
}

int countSWhichDependFromM(int masterPk) {
    FILE* file = fopen(customerPath, "rb");

    CustomerMetaEntity customerMetaEntity;
    if (!readM(&customerMetaEntity, masterPk, NULL, false))
        return -1;

    fclose(file);
    return customerMetaEntity.numOfReviews;
}
