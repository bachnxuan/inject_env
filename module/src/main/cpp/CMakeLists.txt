cmake_minimum_required(VERSION 3.22.1)
project(inject)

set(CXX_FLAGS "${CXX_FLAGS} -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")

add_library(${MODULE_NAME} SHARED inject.cpp ksu_mgr.c)
target_link_libraries(${MODULE_NAME} cxx log)

add_subdirectory(external)
