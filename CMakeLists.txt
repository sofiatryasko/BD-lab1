cmake_minimum_required(VERSION 3.19)
project(databaseLab C)

set(CMAKE_C_STANDARD 11)

add_executable(databaseLab main.c entities/Customer.h entities/CustomerMetaEntity.h entities/Review.h entities/ReviewMetaEntity.h src/FileUtility.c src/FileUtility.h entities/IndexEntity.h src/CRUD.c src/CRUD.h src/MetaDB.c src/MetaDB.h entities/ReviewMetaEntity.c entities/Review.c entities/CustomerMetaEntity.c entities/Customer.c)