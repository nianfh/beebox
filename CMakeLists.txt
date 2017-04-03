cmake_minimum_required(VERSION 3.5)

project(beebox)

add_definitions(-DLINUX)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(${PROJECT_SOURCE_DIR}/include)
set(LIB_SRC
src/Async.cpp           src/HttpServer.cpp  src/Socket.cpp     src/TcpSocket.cpp
src/Utils.cpp           src/Date.cpp        src/LoopTask.cpp   src/Thread.cpp
src/File.cpp            src/Mutex.cpp       src/Task.cpp       src/Timer.cpp
src/HttpConnection.cpp  src/Semaphore.cpp   src/TcpServer.cpp  src/UdpSocket.cpp
)
add_library(beebox STATIC ${LIB_SRC})

link_directories(${PROJECT_SOURCE_DIR}/lib)
set(APP_SRC example/main.cpp example/Beebox.cpp example/BeeboxServer.cpp)
add_executable(beebox.out ${APP_SRC})
target_link_libraries(beebox.out beebox uv sqlite3 http_parser pthread)


