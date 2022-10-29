// Created by Yehor on 08.09.2021.

#ifndef DATABASELAB_CRUD_H
#define DATABASELAB_CRUD_H

#include "stdio.h"

#include "MetaDB.h"
#include "FileUtility.h"
#include "../entities/CustomerMetaEntity.h"
#include "../entities/ReviewMetaEntity.h"
#include "../entities/IndexEntity.h"

extern const char customerPath[];
extern const char reviewPath[];
extern const char indexTablePath[];
extern const char absolutePath[];

void runDataBase();
void stopDataBase();

bool createM(CustomerMetaEntity* data);
bool createS(ReviewMetaEntity* data);

bool readM(CustomerMetaEntity* dest, int pk, int* index, bool toDelete);
bool readS(ReviewMetaEntity* dest, int pk, int* index);

bool updateM(int pk, CustomerMetaEntity* updateData);
bool updateS(int pk, ReviewMetaEntity* updateData);

bool deleteM(int pk);
bool deleteS(int pk);

int countM();
int countS();
int countSWhichDependFromM(int masterPk);

void addIndexEntity(IndexEntity newEntity);

#endif //DATABASELAB_CRUD_H
