cmake_minimum_required (VERSION 2.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

SET(CMAKE_BUILD_TYPE "Release")

rosbuild_init()

rosbuild_add_executable(MeasLoader src/main.cpp)
