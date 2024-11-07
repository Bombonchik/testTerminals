#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>

std::mutex gameMutex;

struct GameState {
    int currentPlayer;
    std::string publicMessage;
};

void saveGameState(const GameState& state) {
    std::lock_guard<std::mutex> lock(gameMutex);
    std::ofstream file("game_state.txt");
    if (file.is_open()) {
        file << state.currentPlayer << "\n";
        file << state.publicMessage << "\n";
        file.close();
    } else {
        std::cerr << "Unable to open file for writing" << std::endl;
    }
}

GameState loadGameState() {
    std::lock_guard<std::mutex> lock(gameMutex);
    GameState state;
    std::ifstream file("game_state.txt");
    if (file.is_open()) {
        file >> state.currentPlayer;
        file.ignore(); // Ignore the newline character after the integer
        std::getline(file, state.publicMessage);
        file.close();
    } else {
        state.currentPlayer = 0;
        state.publicMessage = "Welcome to the game!";
    }
    return state;
}

void displayGameState(const GameState& state, int playerIndex) {
    std::system("clear"); // Use "cls" for Windows, "clear" for Unix/Linux
    std::cout << "Public Message: " << state.publicMessage << std::endl;
    std::cout << "Player " << playerIndex + 1 << ", it's " << (state.currentPlayer == playerIndex ? "your" : "not your") << " turn." << std::endl;
    std::cout << "Your private message: Hello from Player " << playerIndex + 1 << std::endl;
}

void playerTurn(int playerIndex) {
    while (true) {
        GameState state = loadGameState();
        displayGameState(state, playerIndex);

        if (state.currentPlayer == playerIndex) {
            std::cout << "Enter a new public message: ";
            std::string newMessage;
            std::getline(std::cin, newMessage); // Read the whole line including spaces

            state.publicMessage = newMessage;
            state.currentPlayer = (state.currentPlayer + 1) % 4; // Assuming 4 players
            saveGameState(state);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Player index not specified!" << std::endl;
        return 1;
    }

    int playerIndex = std::stoi(argv[1]);

    // Initialize game state for the first player
    if (playerIndex == 0) {
        GameState initialState = {0, "Welcome to the game!"};
        saveGameState(initialState);
    }

    playerTurn(playerIndex);

    return 0;
}
