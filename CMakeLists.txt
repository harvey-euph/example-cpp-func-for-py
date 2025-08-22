cmake_minimum_required(VERSION 3.15)
project(cpp_pybind11_quantlib_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(pybind11 CONFIG REQUIRED)

if(NOT DEFINED ENV{CONDA_PREFIX})
    message(FATAL_ERROR "Please activate your conda environment first.")
endif()

set(QuantLib_INCLUDE_DIRS $ENV{CONDA_PREFIX}/include)
set(QuantLib_LIBRARIES $ENV{CONDA_PREFIX}/lib/libQuantLib.so)

include_directories(${QuantLib_INCLUDE_DIRS})

set(MYLIB_MODULE_NAME mylib)

pybind11_add_module(${MYLIB_MODULE_NAME} src/main.cpp)

target_link_libraries(${MYLIB_MODULE_NAME} PRIVATE ${QuantLib_LIBRARIES})

target_compile_definitions(${MYLIB_MODULE_NAME}
    PRIVATE MYLIB_MODULE_NAME=${MYLIB_MODULE_NAME})