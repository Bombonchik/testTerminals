cmake_minimum_required(VERSION 3.28)
project(testTerminals)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use a relative path to the Conan toolchain file from the build directory
find_package(SQLiteCpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(asio REQUIRED)
find_package(cereal REQUIRED)
find_package(FTXUI REQUIRED
        COMPONENTS
        screen
        dom
        component
)

# Define executables
add_executable(launcher main.cpp
        canasta_console.cpp
        canasta_console.hpp card.cpp card.hpp game_view.cpp game_view.hpp hand.cpp hand.hpp
        client_deck.hpp client_deck.cpp player_public_info.hpp meld.hpp input_guard.hpp input_guard.cpp)

# Link libraries provided by Conan
#target_link_libraries(launcher sqlitecpp sqlite3)
target_link_libraries(launcher SQLiteCpp spdlog::spdlog asio::asio cereal::cereal ftxui::screen ftxui::dom ftxui::component)
#target_link_libraries(game_player SQLiteCpp spdlog::spdlog asio::asio cereal::cereal)
