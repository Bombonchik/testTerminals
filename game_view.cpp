#include "game_view.hpp"


GameView::GameView(): console(), screen(ScreenInteractive::TerminalOutput()) {
    // Initialize the console UI
    //console.clear();
    console.print("Welcome to Canasta!", CanastaConsole::Color::BrightCyan, true);
    console.print("Initializing game view...", CanastaConsole::Color::BrightWhite, true);
}

void GameView::printCard(const Card& card) {
    auto color = card.getColor() == CardColor::RED ? 
    CanastaConsole::Color::BrightRed : CanastaConsole::Color::BrightWhite;
    std::string cartToPrint = "";
    auto rank = card.getRank();
    if (rank == Rank::Joker) {
        //cartToPrint = "☻";
        cartToPrint = "@";
    //} else if (rank >= Rank::Two && rank <= Rank::Ten) {
    //    cartToPrint = std::to_string(static_cast<int>(rank));
    } else if (rank >= Rank::Two && rank <= Rank::Nine) {
        cartToPrint = std::to_string(static_cast<int>(rank));
    } else if (rank == Rank::Ten) {
        cartToPrint = "X";
    } else if (rank == Rank::Jack) {
        cartToPrint = "J";
    } else if (rank == Rank::Queen) {
        cartToPrint = "Q";
    } else if (rank == Rank::King) {
        cartToPrint = "K";
    } else if (rank == Rank::Ace) {
        cartToPrint = "A";
    }
    console.print(cartToPrint, color);
}

void GameView::printHand(const Hand& hand) {
    console.print("Your Hand:", CanastaConsole::Color::BrightCyan, true);
    auto cards = hand.getCards();
    console.print(std::to_string(cards.size()), CanastaConsole::Color::BrightWhite, true);
    if (cards.empty()) {
        console.print("No cards in hand", CanastaConsole::Color::BrightWhite);
        return;
    }
    auto currentRank = cards.front().getRank();
    for (std::size_t i = 0; i < cards.size(); ++i) {
        bool rankWas = false;
        if (cards[i].getRank() == currentRank) {
            rankWas = true;
            printCard(cards[i]);
            if (i < cards.size() - 1) {
                ++i;
            } else {
                break;
            }
        }
        while (cards[i].getRank() == currentRank && i < cards.size()) {
            console.printSpace();
            printCard(cards[i]);
            ++i;
        }
        if (rankWas && i < cards.size()){
            console.print("|");
        }
        currentRank = cards[i].getRank();
        --i;
    }
    console.print("\n", CanastaConsole::Color::BrightCyan);
}

void GameView::printMeld(const std::vector<MeldView>& melds) {
    console.print("Your Meld:", CanastaConsole::Color::BrightCyan, true);
    for (std::size_t i = 0; i < 8; ++i) {
        for (auto& meld : melds) {
            console.print("|");
            if (meld.cards.size() >= 7 && i == 7) {
                console.print("C");
                continue;
            }
            if (i < meld.cards.size()) {
                if (meld.cards.size() < 7) {
                    printCard(meld.cards[i]);
                } else if (i == 0) {
                    printCard(meld.cards.front());
                } else if (i == 1 && meld.cards.front().getRank() != meld.cards.back().getRank()) {
                    printCard(meld.cards.back());
                } else {
                    console.printSpace();
                }
            } else {
                console.printSpace();
            }
        }
        console.print("|");
        console.printNewLine();
    }
}


void GameView::ftxuiPrintMeld(const std::vector<MeldView>& melds) {

    // Build row-by-row exactly like your console version:
    std::vector<Element> rows;
    constexpr size_t maxRows = 8;

    for (size_t row = 0; row < maxRows; ++row) {
        std::vector<Element> cells;

        for (auto& meld : melds) {
            // not first meld
            if (meld.rank != melds.front().rank)
                cells.push_back(text("|"));

            // 1) true canasta indicator: if they have ≥7 cards, on the 8th row show 'C'
            if (meld.cards.size() >= 7 && row == 7) {
                cells.push_back(text(" C ") | color(Color::Grey0) | flex_grow);
                continue;
            }

            // 2) otherwise if this row falls inside the number of cards
            if (row < meld.cards.size()) {
                // a) small meld <7 cards ⇒ show the card at index = row
                if (meld.cards.size() < 7) {
                    cells.push_back(makeCardElement(meld.cards[row]));

                } else {
                    if (row == 0) {
                        cells.push_back(makeCardElement(meld.cards.front()));
                    } else if (row == 1 &&
                            meld.cards.front().getRank() != meld.cards.back().getRank()) {
                        cells.push_back(makeCardElement(meld.cards.back()));
                    } else {
                        cells.push_back(text("   ") | flex_grow);
                    }
                }
            }
            // 3) outside the card count ⇒ blank
            else
                cells.push_back(text("   ") | flex_grow);
        }

        rows.push_back(hbox(std::move(cells)));
    }

    // wrap it in a box exactly like before
    auto grid   = vbox(std::move(rows)) | border;
    auto layout = vbox({
        // text("Your Meld:")    | bold | color(Color::Cyan),
        // separator(),
        grid,
        //text("Press any key…") | dim,
    }) | center;

    // turn into a Component and exit on any key
    Component component = Renderer([&] { return layout; });
    component = CatchEvent(component, [&](Event e) {
        if (e.is_character()) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(component);
    //console.clear();
}

Element GameView::makeMeldGrid(const std::vector<MeldView>& melds) {
    std::vector<Element> rows;
    constexpr size_t maxRows = 8;

    for (size_t row = 0; row < maxRows; ++row) {
        std::vector<Element> cells;

        for (auto& meld : melds) {
            // not first meld
            if (meld.rank != melds.front().rank)
                cells.push_back(text("|"));

            // 1) true canasta indicator: if they have ≥7 cards, on the 8th row show 'C'
            if (meld.cards.size() >= 7 && row == 7) {
                cells.push_back(text(" C ") | color(Color::Grey0) | flex_grow);
                continue;
            }

            // 2) otherwise if this row falls inside the number of cards
            if (row < meld.cards.size()) {
                // a) small meld <7 cards ⇒ show the card at index = row
                if (meld.cards.size() < 7) {
                    cells.push_back(makeCardElement(meld.cards[row]));

                } else {
                    if (row == 0) {
                        cells.push_back(makeCardElement(meld.cards.front()));
                    } else if (row == 1 &&
                            meld.cards.front().getRank() != meld.cards.back().getRank()) {
                        cells.push_back(makeCardElement(meld.cards.back()));
                    } else {
                        cells.push_back(text("   ") | flex_grow);
                    }
                }
            }
            // 3) outside the card count ⇒ blank
            else
                cells.push_back(text("   ") | flex_grow);
        }

        rows.push_back(hbox(std::move(cells)));
    }

    // wrap it in a box exactly like before
    return vbox(std::move(rows)) | border;
}

// Demo
Element GameView::makeHandRow(const Hand& hand) {
    std::vector<Element> cards;
    for (auto& c : hand.getCards())
        cards.push_back(makeCardElement(c));
    return hbox(std::move(cards)) | border;
}

// Demo
Element GameView::makeDeckInfo(const ClientDeck& deck) {
    bool hasTop = deck.getTopDiscardCard().has_value();
    auto topCard = hasTop ? deck.getTopDiscardCard().value() : Card();
    return vbox({
        hbox({
            text("Main Deck Size:  ") | bold,
            text(std::to_string(deck.getMainDeckSize())) | bold,
        }),
        hbox({
            text("Top Discard Card:") | bold,
            hasTop ? makeCardElement(topCard) : text(" ") | bold,
        }),
    });
}

CardView GameView::getCardView(const Card& card) {
    CardView cardView;
    cardView.color = card.getColor() == CardColor::RED ? 
    Color::RedLight : Color::White;
    std::string cardToPrint = "";
    auto rank = card.getRank();
    if (rank == Rank::Joker) {
        cardToPrint = "@";
    } else if (rank >= Rank::Two && rank <= Rank::Nine) {
        cardToPrint = std::to_string(static_cast<int>(rank));
    } else if (rank == Rank::Ten) {
        cardToPrint = "X";
    } else if (rank == Rank::Jack) {
        cardToPrint = "J";
    } else if (rank == Rank::Queen) {
        cardToPrint = "Q";
    } else if (rank == Rank::King) {
        cardToPrint = "K";
    } else if (rank == Rank::Ace) {
        cardToPrint = "A";
    }
    cardView.label = cardToPrint;
    return cardView;
}

Element GameView::makeCardElement(const Card& card, bool padded) {
    // 1) pull your CardView
    CardView cv = getCardView(card);

    // 2) pick whether to pad or not
    std::string label = padded
    ? " " + cv.label + " "
    : cv.label;

    // 3) build & return the styled element
    return text(label)
        | color(cv.color)
        | flex_grow;
}

std::string GameView::promptStringWithBoard(const std::string& question,
    std::string& placeholder, const BoardState& boardState) {
    // 1) Your local buffer
    std::string buffer;

    // 2) Configure InputOption to *reference* that buffer
    InputOption option = InputOption::Default();
    option.content     = &buffer;             // <<–– bind to your std::string
    option.placeholder = StringRef(placeholder);        // <<–– bind to the caller’s placeholder
    option.on_enter    = [&] { screen.ExitLoopClosure()(); };

    // 3) Build & render
    auto input    = Input(option);
    auto renderer = Renderer(input, [&] {
        return vbox({
            text(question),
            input->Render() | frame,
            text("(press Enter)") | dim
        }) | border | size(WIDTH, LESS_THAN, 20);
    });

    screen.Loop(renderer);
    //console.clear();
    // 4) buffer now holds the final text
    return buffer.substr(0, buffer.size() - 1);
}

int GameView::promptChoiceWithBoard(const std::string& question,
    const std::vector<std::string>& options, const BoardState& boardState) {
    auto board = makeBoard(boardState) | flex_grow;
    // 1) Local selected index
    int selected = 0;

    // 2) Configure RadioboxOption to *reference* it
    RadioboxOption opt = RadioboxOption::Simple();
    opt.entries  = options;        // can assign vector<string> directly
    opt.selected = &selected;      // <<–– bind to your int
    // opt.on_change can stay default

    // 3) Build & render
    auto menu = Radiobox(opt);
    auto okButton = Button(" OK ", screen.ExitLoopClosure());
    //auto container = Container::Vertical({radiobox, button});
    auto promptBox = vbox({
            text(question),
            menu->Render() | frame,
            okButton->Render() | center
        }) | border
        | size(HEIGHT, EQUAL, 6);
    auto root = vbox({ board, promptBox });
    Component component = Renderer([&] { return root; });
    screen.Loop(component);

    //console.clear();
    return selected;
}

Element GameView::makeBoard(const BoardState& boardState) {
    auto myMeldGrid = makeMeldGrid(boardState.myTeamMelds);
    auto opponentMeldGrid = makeMeldGrid(boardState.opponentTeamMelds);
    auto myHandRow = makeHandRow(boardState.myHand);
    auto deckInfo = makeDeckInfo(boardState.deckState);
    auto scoreInfo = makeScoreInfo(boardState.myTeamTotalScore,
        boardState.opponentTeamTotalScore,
        boardState.myTeamMeldPoints,
        boardState.opponentTeamMeldPoints);
    auto myPlayerInfo = makePlayerInfo(boardState.myPlayer);
    auto opponentPlayerInfo = makePlayerInfo(boardState.oppositePlayer);
    auto leftPlayerInfo = boardState.leftPlayer.has_value() ?
        makePlayerInfo(boardState.leftPlayer.value()) : text(" ");
    auto rightPlayerInfo = boardState.rightPlayer.has_value() ?
        makePlayerInfo(boardState.rightPlayer.value()) : text(" ");

    // 4) Combine in a vertical stack
    return vbox({
        hbox({
            opponentPlayerInfo,
        }) | center,
        hbox({
            hbox({ filler()|flex, myMeldGrid,        filler()|flex }) | flex,
            hbox({ filler()|flex, opponentMeldGrid, filler()|flex }) | flex
        }),
        hbox({
            leftPlayerInfo  | flex,
            scoreInfo  | flex,
            deckInfo  | flex,
            rightPlayerInfo | align_right,
        }),
        hbox({
            myHandRow,
        }) | center,
        hbox({
            myPlayerInfo,
        }) | center
    });
}

Element GameView::makeScoreInfo(int myTeamTotalScore, int opponentTeamTotalScore,
    int myTeamMeldPoints, int opponentTeamMeldPoints) {
    return vbox({
        hbox({
            text("My Team: ") | bold,
            text(std::to_string(myTeamTotalScore)) | bold,
            text(" (Meld: ") | bold,
            text(std::to_string(myTeamMeldPoints)) | bold,
            text(")"),
        }),
        hbox({
            text("Opponent Team: ") | bold,
            text(std::to_string(opponentTeamTotalScore)) | bold,
            text(" (Meld: ") | bold,
            text(std::to_string(opponentTeamMeldPoints)) | bold,
            text(")"),
        }),
    });
}

Element GameView::makePlayerInfo(const PlayerPublicInfo& player) {
    return hbox({
        text(player.name) | bold | (player.isCurrentTurn ? color(Color::Cyan) : color(Color::White)),
        text(", " + std::to_string(player.handCardCount)) | bold | (player.isCurrentTurn ? color(Color::Cyan) : color(Color::White)),
    });
}