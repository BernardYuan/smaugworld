cmake_minimum_required(VERSION 3.5)
project(smaugworld)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    cleanSem.sh
    cow.c
    cow.h
    Makefile
    sheep.c
    sheep.h
    smaug.c
    smaug.h
    smaugworld.c
    smaugworld.h hunter.c hunter.h thief.c thief.h B/smaugworld.c B/smaugworld.h B/smaug.c B/smaug.h B/cow.c B/cow.h B/sheep.c B/sheep.h)

add_executable(smaugworld ${SOURCE_FILES})