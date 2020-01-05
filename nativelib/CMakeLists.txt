cmake_minimum_required(VERSION 3.9)
project(nativelib)

set(CMAKE_CXX_STANDARD 11)
include_directories($ENV{JAVA_HOME}/include $ENV{JAVA_HOME}/include/linux)

#set_target_properties(std_image PROPERTIES LINKER_LANGUAGE C)

add_library(realsensej SHARED src/include/stb_image_write.h src/com_unigent_realsensej_RealSenseJ.cpp)
target_link_libraries(realsensej realsense2)

add_executable(camera_test src/camera_test.cpp)
target_link_libraries(camera_test realsense2)
