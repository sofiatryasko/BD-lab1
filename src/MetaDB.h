// Created by Yehor on 11.09.2021.

#ifndef DATABASELAB_METADB_H
#define DATABASELAB_METADB_H

#include <stdio.h>
#include <malloc.h>

extern const char metaPath[];

typedef struct Pair {
    int masterNumDeleted;
    int slaveNumDeleted;
} Pair;

void setNumDeleted(int masterNumDeleted, int slaveNumDeleted);
void getNumDeleted(int* masterNumDeleted, int* slaveNumDeleted);

#endif //DATABASELAB_METADB_H
