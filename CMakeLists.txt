cmake_minimum_required(VERSION 3.28)
project(testTerminals)

set(CMAKE_CXX_STANDARD 23)

# Create the launcher executable
add_executable(launcher main.cpp)

# Create the game player executable
add_executable(game_player game_player.cpp)
