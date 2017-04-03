cmake_minimum_required(VERSION 3.5)

PROJECT(sqlite3)

include_directories(${PROJECT_SOURCE_DIR}/src)
set(LIB_SRC src/sqlite3.c)
add_library(sqlite3 STATIC ${LIB_SRC})


