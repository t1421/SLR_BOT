cmake_minimum_required(VERSION 3.22)

project(MIS_BOT)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "K:/Sound_Server/lib/boost_1_78_0")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "K:/Sound_Server/lib/boost_1_78_0/libs")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -Wall")

add_executable(MIS_BOT
    MIS_BOT.cpp
	src/Broker.cpp
	src/CardBaseSMJ.cpp
	src/DEBUG.cpp
	src/FireBot.cpp
	src/LOAD.cpp
	src/Util.cpp
)

include_directories(incl)
include_directories(API/API)
include_directories(${CMAKE_INCLUDE_PATH})

find_package(Boost 1.74 REQUIRED)
target_link_libraries(MIS_BOT LINK_PUBLIC ${Boost_LIBRARIES})
