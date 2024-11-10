#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <asio.hpp>
#include "game_state.hpp"
#include "spdlog/spdlog.h"
#include "cereal/archives/binary.hpp"

// Server constants
constexpr int PORT = 12345;

// Launch a terminal for each player
void launchTerminal(int playerIndex) {
    std::string command;
#ifdef _WIN32
    command = "start cmd /k \"debug\\game_player.exe " + std::to_string(playerIndex) + "\"";
#elif __APPLE__
    command = "osascript -e 'tell application \"Terminal\" to do script \"cd '$PWD' && ./game_player " + std::to_string(playerIndex) + "\"'";
#else
    command = "gnome-terminal -- bash -c './game_player " + std::to_string(playerIndex) + "; exec bash'";
#endif
    spdlog::info("Launching terminal for Player {}", playerIndex + 1);
    system(command.c_str());
}

// Handle client connections
void handleClient(asio::ip::tcp::socket socket, GameState& gameState) {
    try {
        spdlog::info("Client connected: {}", socket.remote_endpoint().address().to_string());
        while (true) {
            // Serialize and send the game state
            std::ostringstream os;
            {
                cereal::BinaryOutputArchive archive(os);
                archive(gameState);
            }
            std::string serializedData = os.str();
            asio::write(socket, asio::buffer(serializedData));

            // Log the broadcasted state
            spdlog::debug("Broadcasting GameState: [Player {}] {}", gameState.currentPlayer, gameState.publicMessage);

            // Sleep to simulate updates
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    } catch (const std::exception& e) {
        spdlog::error("Client disconnected: {}", e.what());
    }
}

// Start the server
void startServer(GameState& gameState, int numPlayers) {
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), PORT));

    spdlog::info("Server started on port {}", PORT);

    for (int i = 0; i < numPlayers; ++i) {
        asio::ip::tcp::socket socket(io_context);
        acceptor.accept(socket);
        spdlog::info("Player {} connected.", i + 1);
        std::thread(handleClient, std::move(socket), std::ref(gameState)).detach();
    }
}

int main() {
    int numPlayers = 4; // Number of players
    GameState gameState = {0, "Welcome to the game!"}; // Initial game state

    spdlog::info("Launching {} player terminals...", numPlayers);
    for (int i = 0; i < numPlayers; ++i) {
        launchTerminal(i);
    }

    spdlog::info("Starting the server...");
    startServer(gameState, numPlayers);

    // Keep the main thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
