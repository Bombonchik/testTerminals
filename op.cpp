#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <libgen.h>
#endif

std::string getExecutableDirectory() {
    char path[1024];
#ifdef _WIN32
    GetModuleFileName(NULL, path, sizeof(path));
#else
    ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
    if (count != -1) {
        path[count] = '\0';
        return std::string(dirname(path));
    }
#endif
    return "";
}

void launchTerminal(int playerIndex) {
    std::string path = getExecutableDirectory();
    std::string command;
#ifdef _WIN32
    command = "start cmd /k \"cd /d " + path + " && game_player.exe " + std::to_string(playerIndex) + "\"";
#elif __APPLE__
    command = "osascript -e 'tell application \"Terminal\" to do script \"cd " + path + " && ./game_player " + std::to_string(playerIndex) + "\"'";
#else
    command = "gnome-terminal -- bash -c 'cd " + path + " && ./game_player " + std::to_string(playerIndex) + "; exec bash'";
#endif
    system(command.c_str());
}

void detectOSAndLaunchTerminals(int numPlayers) {
    for (int i = 0; i < numPlayers; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Add delay to ensure terminals launch correctly
        launchTerminal(i);
    }
}

int main() {
    int numPlayers = 4; // Example for 4 players
    detectOSAndLaunchTerminals(numPlayers);
    return 0;
}
