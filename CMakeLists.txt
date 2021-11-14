cmake_minimum_required(VERSION 3.20.5)
project(airport)

include_directories(headers)

file(GLOB SOURCES "src/*.cpp")

add_executable(airport ${SOURCES})
