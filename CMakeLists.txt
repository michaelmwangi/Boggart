cmake_minimum_required(VERSION 3.0)
project(boggart)
add_definitions(-std=c++11)
add_executable(boggart main.cpp network/server.cpp)