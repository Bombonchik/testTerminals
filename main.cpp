#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <sstream>
#include "SQLiteCpp/SQLiteCpp.h"
#include <spdlog/spdlog.h>
#include "asio.hpp"
#include "cereal/archives/binary.hpp"
#include <cereal/types/string.hpp>
#include "canasta_console.hpp"
#include "card.hpp"
#include "game_view.hpp"
#include "hand.hpp"
#include <random>
//#include "cereal/cereal.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif


// Launch a terminal for each player
void launchTerminal(int playerIndex) {
    std::string command;
#ifdef _WIN32
    // Windows: Use 'start' to open a new command prompt
    command = "start cmd /k \"game_player.exe " + std::to_string(playerIndex) + "\"";
#elif __APPLE__
    // macOS: Use 'osascript' to run a command in a new Terminal window
    command = "osascript -e 'tell application \"Terminal\" to do script \"cd '$PWD' && ./game_player " + std::to_string(playerIndex) + "\"'";
#else
    // Linux: Use 'gnome-terminal' or 'xterm' to open a new terminal window
    command = "gnome-terminal -- bash -c './game_player " + std::to_string(playerIndex) + "; exec bash'";
#endif
    system(command.c_str());
}

bool randomBool() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::bernoulli_distribution dist(1.0/3.0);          // p = 1/3 for true

    // 2) Generate
    return dist(gen);
}

void testCanastaConsole() {
    CanastaConsole ui;
    ui.clear();
    for (int i = 0; i < 20; ++i) {
        ui.print("Testing CanastaConsole: " + std::to_string(i), CanastaConsole::Color::BrightCyan);
    }
    ui.clear();

    ui.print("CanastaConsole Test Harness", CanastaConsole::Color::BrightMagenta);
    ui.print("--------------------------------", CanastaConsole::Color::BrightWhite);

    // Demonstrate colors
    ui.print("Default color text");
    ui.print("Red message", CanastaConsole::Color::Red);
    ui.print("Green message", CanastaConsole::Color::Green);
    ui.print("Yellow message", CanastaConsole::Color::Yellow);
    ui.print("Blue message", CanastaConsole::Color::Blue);
    ui.print("Magenta message", CanastaConsole::Color::Magenta);
    ui.print("Cyan message", CanastaConsole::Color::Cyan);
    ui.print("White message", CanastaConsole::Color::White);

    ui.print("BrightRed message", CanastaConsole::Color::BrightRed);
    ui.print("BrightGreen message", CanastaConsole::Color::BrightGreen);
    ui.print("BrightYellow message", CanastaConsole::Color::BrightYellow);
    ui.print("BrightBlue message", CanastaConsole::Color::BrightBlue);
    ui.print("BrightMagenta message", CanastaConsole::Color::BrightMagenta);
    ui.print("BrightCyan message", CanastaConsole::Color::BrightCyan);
    ui.print("BrightWhite message", CanastaConsole::Color::BrightWhite);

    // Demonstrate UTF-8
    ui.print("UTF-8 symbols: ✔ ✓ → ← ▲ ▼ ★ ☆ ☯ ☢", CanastaConsole::Color::BrightCyan);
    ui.print("Unicode emojis: 😀 🎉 🚀 🃏", CanastaConsole::Color::Yellow);
    ui.print("CJK characters: 漢字 仮名 가나다", CanastaConsole::Color::Green);

    // Demonstrate printing without newline
    ui.print("Loading: [", CanastaConsole::Color::BrightBlue, false);
    for (int i = 0; i <= 10; ++i) {
        ui.print(std::to_string(i * 10) + "%", CanastaConsole::Color::BrightGreen, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ui.print("Loading: [" + std::to_string((i+1)*10) + "%] ", CanastaConsole::Color::BrightBlue, false);
    }
    ui.print("] Done!", CanastaConsole::Color::BrightGreen);

    // Pause before exit
    ui.print("Press Enter to exit...", CanastaConsole::Color::BrightWhite, false);

}

void testPrintCard(GameView& gameView, CanastaConsole& console) {
    Card card;
    for (size_t i = 1; i < 15; ++i) {
        card = Card(static_cast<Rank>(i), CardColor::RED);
        gameView.printCard(card);
        card = Card(static_cast<Rank>(i), CardColor::BLACK);
        gameView.printCard(card);
    }
    console.printNewLine();
}

Hand getTestHand() {
    Hand hand;
    for (size_t i = 1; i < 15; ++i) {
        if (randomBool())
            hand.addCard(Card(static_cast<Rank>(i), CardColor::RED));
        if (randomBool())
            hand.addCard(Card(static_cast<Rank>(i), CardColor::RED));
        if (randomBool())
            hand.addCard(Card(static_cast<Rank>(i), CardColor::BLACK));
        if (randomBool())
            hand.addCard(Card(static_cast<Rank>(i), CardColor::BLACK));
    }
    return hand;
}

void testPrintHand(GameView& gameView) {
    gameView.printHand(getTestHand());
}

std::vector<MeldView> getTestMelds() {
    std::vector<MeldView> melds;
    for (size_t i = 4; i < 15; ++i) {
        MeldView meld;
        meld.rank = static_cast<Rank>(i);
        if (randomBool() && randomBool() && randomBool())
            meld.cards.push_back(Card(Rank::Joker, CardColor::RED));
        if (randomBool() && randomBool() && randomBool())
            meld.cards.push_back(Card(Rank::Joker, CardColor::BLACK));
        if (randomBool() && randomBool())
            meld.cards.push_back(Card(Rank::Two, CardColor::RED));
        if (randomBool() && randomBool())
            meld.cards.push_back(Card(Rank::Two, CardColor::BLACK));
        if (randomBool())
            meld.cards.push_back(Card(static_cast<Rank>(i), CardColor::RED));
        if (randomBool())
            meld.cards.push_back(Card(static_cast<Rank>(i), CardColor::RED));
        if (randomBool())
            meld.cards.push_back(Card(static_cast<Rank>(i), CardColor::RED));
        if (randomBool())
            meld.cards.push_back(Card(static_cast<Rank>(i), CardColor::RED));
        // if (randomBool())
        //     meld.cards.push_back(Card(static_cast<Rank>(i), CardColor::RED));
        // if (randomBool())
        //     meld.cards.push_back(Card(static_cast<Rank>(i), CardColor::BLACK));
        if (randomBool())
            meld.cards.push_back(Card(static_cast<Rank>(i), CardColor::BLACK));
        if (randomBool())
            meld.cards.push_back(Card(static_cast<Rank>(i), CardColor::BLACK));
        if (randomBool())
            meld.cards.push_back(Card(static_cast<Rank>(i), CardColor::BLACK));
        if (randomBool())
            meld.cards.push_back(Card(static_cast<Rank>(i), CardColor::BLACK));
        if (meld.cards.size() >= 3) 
            melds.push_back(meld);
    }
    return melds;
}

void testPrintMeld(GameView& gameView, CanastaConsole& console) {
    std::vector<MeldView> melds = getTestMelds();
    console.printNewLine();
    for (auto& meld : melds) {
        for (auto& card : meld.cards) {
            gameView.printCard(card);
            console.printSpace();
        }
        console.printNewLine();
    }
    gameView.printMeld(melds);
    gameView.ftxuiPrintMeld(melds);
}

/*void testPromts(GameView& gameView, CanastaConsole& console) {
    std::string question = "What is your name?";
    std::string placeholder = "Enter your name";
    std::string answer = gameView.promptString(question, placeholder);

    std::vector<std::string> options = {"Option 1", "Option 2", "Option 3"};
    int choice = gameView.promptChoice("Choose an option:", options);
    if (answer[answer.size() - 1] == '\n') {
        console.print("yes", CanastaConsole::Color::BrightCyan);
    }
    console.print("Your answer: " + answer, CanastaConsole::Color::BrightCyan, true);
    console.print("You chose: " + options[choice], CanastaConsole::Color::BrightCyan, true);
}*/

BoardState getTestBoardState() {
    BoardState boardState;
    boardState.myTeamMelds = getTestMelds();
    boardState.opponentTeamMelds = getTestMelds();
    boardState.myHand = getTestHand();
    boardState.deckState = ClientDeck(20, Card(Rank::Five, CardColor::RED), 5, false);
    boardState.myPlayer = PlayerPublicInfo{"Main Man", 13, false};
    boardState.oppositePlayer = PlayerPublicInfo{"Front Man", 9, true};
    boardState.leftPlayer = PlayerPublicInfo{"Left Man", 14, false};
    boardState.rightPlayer = PlayerPublicInfo{"Right Man", 7, false};
    boardState.myTeamTotalScore = 590;
    boardState.opponentTeamTotalScore = 375;
    boardState.myTeamMeldPoints = 70;
    boardState.opponentTeamMeldPoints = 110;
    return boardState;
}

void testPromptChoiceWithBoard(GameView& gameView, CanastaConsole& console) {
    // std::string question = "What is your name?";
    // std::string placeholder = "Enter your name";
    // std::string answer = gameView.promptStringWithBoard(question, placeholder, BoardState());

    std::vector<std::string> options = {"Option 1", "Option 2", "Option 3"};
    int choice = gameView.promptChoiceWithBoard("Choose an option:", options, getTestBoardState());

    //console.print("Your answer: " + answer, CanastaConsole::Color::BrightCyan, true);
    console.print("You chose: " + options[choice], CanastaConsole::Color::BrightCyan, true);
}

int main() {
    int numPlayers = 4;
    GameView gameView;
    CanastaConsole console;

    //testCanastaConsole();
    //testPrintCard(gameView);
    //testPrintHand(gameView);
    //testPrintMeld(gameView, console);
    testPromptChoiceWithBoard(gameView, console);
    // for (int i = 0; i < numPlayers; ++i) {
    //     launchTerminal(i);
    // }


    return 0;
}