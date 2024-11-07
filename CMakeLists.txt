cmake_minimum_required(VERSION 3.28)
project(testTerminals)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use a relative path to the Conan toolchain file from the build directory
find_package(SQLiteCpp REQUIRED)

# Define your executables
add_executable(launcher main.cpp)
add_executable(game_player game_player.cpp)

# Link libraries provided by Conan
#target_link_libraries(launcher sqlitecpp sqlite3)
target_link_libraries(launcher SQLiteCpp)
target_link_libraries(game_player SQLiteCpp)
