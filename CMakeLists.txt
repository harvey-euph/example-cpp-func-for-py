cmake_minimum_required(VERSION 3.15)
project(cpp_pybind11_quantlib_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 輸出路徑
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 找 pybind11
find_package(pybind11 CONFIG REQUIRED)

# QuantLib include & lib
if(NOT DEFINED ENV{CONDA_PREFIX})
    message(FATAL_ERROR "Please activate your conda environment first.")
endif()

set(QuantLib_INCLUDE_DIRS $ENV{CONDA_PREFIX}/include)
set(QuantLib_LIBRARIES $ENV{CONDA_PREFIX}/lib/libQuantLib.so)

include_directories(${QuantLib_INCLUDE_DIRS})

# 建立 pybind11 模組
pybind11_add_module(mymodule src/main.cpp)

# 連結 QuantLib
target_link_libraries(mymodule PRIVATE ${QuantLib_LIBRARIES})
