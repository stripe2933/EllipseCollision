cmake_minimum_required(VERSION 3.24)
project(EllipseCollision)

set(CMAKE_CXX_STANDARD 20)

add_executable(EllipseCollision main.cpp optimization.hpp EllipseShape.cpp EllipseShape.hpp EllipseDescriptor.cpp EllipseDescriptor.hpp FontManager.hpp FontManager.cpp)

find_package(SFML COMPONENTS Graphics REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(fmt REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC SFML::Graphics Eigen3::Eigen fmt::fmt)