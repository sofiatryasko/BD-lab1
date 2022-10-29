// Created by Yehor on 11.09.2021

#include "MetaDB.h"

const char metaPath[] = "C:\\Users\\Yehor\\CLionProjects\\databaseLab\\tables\\meta.bin";

void setNumDeleted(int masterNumDeleted, int slaveNumDeleted) {
    Pair pair;
    pair.masterNumDeleted = masterNumDeleted;
    pair.slaveNumDeleted = slaveNumDeleted;

    FILE* file = fopen(metaPath, "wb");
    fwrite(&pair, sizeof(Pair), 1, file);
    fclose(file);
}

void getNumDeleted(int* masterNumDeleted, int* slaveNumDeleted) {
    Pair pair;
    FILE* file = fopen(metaPath, "rb");
    fread(&pair, sizeof(Pair), 1, file);
    fclose(file);

    *masterNumDeleted = pair.masterNumDeleted;
    *slaveNumDeleted = pair.slaveNumDeleted;
}