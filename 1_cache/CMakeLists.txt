cmake_minimum_required(VERSION 3.10)
project(CachePr)

add_executable(${PROJECT_NAME} "cache.cpp")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra)