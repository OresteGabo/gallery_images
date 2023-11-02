cmake_minimum_required(VERSION 3.26)
project(gallery_art)

set(CMAKE_CXX_STANDARD 17)
# Find OpenCV package
find_package(OpenCV REQUIRED)

add_executable(gallery_art main.cpp)
target_link_libraries(gallery_art ${OpenCV_LIBS})