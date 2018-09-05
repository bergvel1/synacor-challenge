cmake_minimum_required(VERSION 3.12)
project (synacor C)
set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB SOURCES "src/*.c")
file(GLOB HEADERS "include/*.h")

add_executable(synacor_run ${SOURCES})
target_link_libraries(synacor_run Threads::Threads ${CURSES_LIBRARIES} form)

add_library(synacor STATIC ${SOURCES} ${HEADERS})

add_subdirectory(synacor_tests)

target_link_libraries(synacor_run synacor)




