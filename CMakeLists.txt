cmake_minimum_required(VERSION 3.15)
project(SPLFLIX)

set(CMAKE_CXX_STANDARD 11)

add_executable(SPLFLIX src/Main.cpp src/Watchable.cpp Include/Watchable.h Include/User.h Include/Session.h Include/json.hpp Include/Action.h src/Session.cpp src/Action.cpp src/User.cpp)

