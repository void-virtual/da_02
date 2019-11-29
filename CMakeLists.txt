cmake_minimum_required(VERSION 3.14)
project(da_02)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")

add_executable(da_02 main.cpp TPatricia.cpp TPatricia.h TVector.h TOptional.h)

add_executable(test test_generator.cpp TPatricia.cpp profile.h)

add_executable(copy_test copy_test.cpp)

add_executable(text_test text_test_generator.cpp)