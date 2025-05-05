#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include <string>
#include <vector>
#include <atomic>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/loop.hpp>
#include <optional>
#include <map>
#include "card.hpp"
#include "hand.hpp"
#include "canasta_console.hpp"
#include "client_deck.hpp"
#include "player_public_info.hpp"
#include "meld.hpp"
#include "input_guard.hpp"
#include "fmt/format.h"
#include <algorithm>

struct CardView {
    std::string label;
    ftxui::Color color;
};

struct MeldView {
    Rank rank;
    std::vector<Card> cards;
};

struct BoardState {
    std::vector<MeldView> myTeamMelds;
    std::vector<MeldView> opponentTeamMelds;
    Hand myHand;
    ClientDeck deckState;
    PlayerPublicInfo myPlayer;
    PlayerPublicInfo oppositePlayer;
    std::optional<PlayerPublicInfo> leftPlayer;
    std::optional<PlayerPublicInfo> rightPlayer;
    int myTeamTotalScore;
    int opponentTeamTotalScore;
    int myTeamMeldPoints;
    int opponentTeamMeldPoints;
};

using namespace ftxui;

class GameView {
public:
    // Constructor
    GameView();
// private:
    void printCard(const Card& card);
    void printHand(const Hand& hand);
    void printMeld(const std::vector<MeldView>& melds);
    void ftxuiPrintMeld(const std::vector<MeldView>& melds);
    CardView getCardView(const Card& card);
    std::string promptString(const std::string& question, std::string& placeholder);
    void showStaticBoardWithMessages(
        const std::vector<std::string>& messages, const BoardState& boardState);
    int promptChoiceWithBoard(const std::string& question, const std::vector<std::string>& options,
        const BoardState& boardState, std::optional<const std::string> message = std::nullopt);
    std::vector<MeldRequest> runMeldWizard(const BoardState& boardState);
    Card runDiscardWizard(const BoardState& boardState);
    void restoreInput();
private:
    CanastaConsole console;
    ScreenInteractive screen;
    std::optional<InputGuard> inputGuard;

    Element makeCardElement(const Card& card, bool padded = true);
    Element makeBoard(const BoardState& boardState);
    Element makeHandGrid(const Hand& hand);
    Element makeDeckInfo(const ClientDeck& deck);
    Element makeScoreInfo(int myTeamTotalScore, int opponentTeamTotalScore,
        int myTeamMeldPoints, int opponentTeamMeldPoints, Color textColor1, Color textColor2);
    Element makeMeldGrid(const std::vector<MeldView>& melds, Color frameColor);
    Element makePlayerInfo(const PlayerPublicInfo& player);
    void disableInput();
};

#endif // GAME_VIEW_HPP