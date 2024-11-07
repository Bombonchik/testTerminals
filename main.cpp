#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "SQLiteCpp/SQLiteCpp.h"

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

int main() {
    int numPlayers = 4; // Example for 4 players
    check_db();
    //detectOSAndLaunchTerminals(numPlayers);
    // Main game logic
    return 0;
}
