cmake_minimum_required(VERSION 3.0)
project(cubiomes-cpp-test)

execute_process(COMMAND brew --prefix OUTPUT_VARIABLE brew_prefix RESULT_VARIABLE brew_prefix_result)
string(STRIP "${brew_prefix}" brew_prefix)
if (EXISTS "${brew_prefix}")
    include_directories(${brew_prefix}/include)
    link_directories(${brew_prefix}/lib)
endif()

if (EXISTS "$ENV{HOMEPATH}/boost_1_72_0")
    include_directories($ENV{HOMEPATH}/boost_1_72_0)
    link_directories($ENV{HOMEPATH}/boost_1_72_0/stage/lib)
endif()

if (EXISTS "$ENV{CUDA_PATH}")
    include_directories($ENV{CUDA_PATH}/include)
    link_directories($ENV{CUDA_PATH}/lib/x64)
endif()

include_directories(..
                    ../ext/doctest/doctest)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
add_executable(cubiomes-cpp-test main.cpp
                                 ../layers.c
                                 ../layers_nr.cpp
                                 ../layers_nr.hpp
                                 ../layers.h
                                 ../generator.h
                                 ../finders.h
                                 ../generator.c
                                 ../finders.c
                                 ../memory.cpp
                                 ../memory.hpp
                                 ../layers.cl)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
    set(CMAKE_EXE_LINKER_FLAGS "-framework OpenCL ${CMAKE_EXE_LINKER_FLAGS}")
else()
    list(APPEND cubiomes_cpp_test_link_libraries OpenCL)
endif()

add_definitions(-DBOOST_COMPUTE_USE_CPP11)
target_link_libraries(cubiomes-cpp-test ${cubiomes_cpp_test_link_libraries})
