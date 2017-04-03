cmake_minimum_required(VERSION 3.5)

PROJECT(http_parser)

include_directories(${PROJECT_SOURCE_DIR}/http-parser)
set(LIB_SRC http-parser/http_parser.c)
add_library(http_parser STATIC ${LIB_SRC})


