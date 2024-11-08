#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include "SQLiteCpp/SQLiteCpp.h"
#include "spdlog/spdlog.h"
#include "asio.hpp"
#include "cereal/archives/binary.hpp"
#include <cereal/types/string.hpp>
//#include "cereal/cereal.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void launchTerminal(int playerIndex) {
    std::string command;
#ifdef _WIN32
    // Windows: Use 'start' to open a new command prompt
    command = "start cmd /k \"debug\\game_player.exe " + std::to_string(playerIndex) + "\"";
#elif __APPLE__
    // macOS: Use 'osascript' to run a command in a new Terminal window
    command = "osascript -e 'tell application \"Terminal\" to do script \"cd '$PWD' && ./game_player " + std::to_string(playerIndex) + "\"'";
#else
    // Linux: Use 'gnome-terminal' or 'xterm' to open a new terminal window
    command = "gnome-terminal -- bash -c './game_player " + std::to_string(playerIndex) + "; exec bash'";
#endif
    system(command.c_str());
}

void detectOSAndLaunchTerminals(int numPlayers) {
    for (int i = 0; i < numPlayers; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Add delay to ensure terminals launch correctly
        launchTerminal(i);
    }
}

struct GameState {
    int currentPlayer;
    std::string publicMessage;

    template <class Archive>
    void serialize(Archive& archive) {
        archive(currentPlayer, publicMessage);
    }
};

void check_db() {
    try {
        // Create or open a database file
        SQLite::Database db("example.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        // Create a new table
        db.exec("CREATE TABLE IF NOT EXISTS user (id INTEGER PRIMARY KEY, name TEXT);");

        // Insert a row into the table
        SQLite::Statement insert(db, "INSERT INTO user (name) VALUES (?)");
        insert.bind(1, "Alice");
        insert.exec();

        // Query the data
        SQLite::Statement query(db, "SELECT id, name FROM user");
        while (query.executeStep()) {
            std::cout << "User: " << query.getColumn(0) << ", " << query.getColumn(1) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void check_logging() {
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");

    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::debug("This message should be displayed..");

    // change log pattern
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    // Compile time log levels
    // Note that this does not change the current log level, it will only
    // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");
}

// Simple function to test Asio functionality
void check_asio() {
    try {
        asio::io_context io_context;

        // Create a timer that expires in 1 second
        asio::steady_timer timer(io_context, std::chrono::seconds(1));
        timer.async_wait([](const asio::error_code& error) {
            if (!error) {
                std::cout << "Asio works! Timer expired after 1 second." << std::endl;
            } else {
                std::cerr << "Timer error: " << error.message() << std::endl;
            }
        });

        // Run the io_context to process the timer
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Asio error: " << e.what() << std::endl;
    }
}

void check_cereal() {
    // Serialization
    std::ostringstream os;
    {
        cereal::BinaryOutputArchive archive(os);
        archive(GameState{1, "Hello!"});
    }
    std::string serializedData = os.str();

    // Deserialization
    std::istringstream is(serializedData);
    GameState deserializedState;
    {
        cereal::BinaryInputArchive archive(is);
        archive(deserializedState);
    }
    spdlog::info("{} {}", deserializedState.currentPlayer, deserializedState.publicMessage);
}

int main() {
    int numPlayers = 4; // Example for 4 players
    check_db();
    check_logging();
    check_asio();
    check_cereal();
    //detectOSAndLaunchTerminals(numPlayers);
    // Main game logic
    return 0;
}
