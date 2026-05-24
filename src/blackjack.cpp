#include "../include/game.h"

// ============================================================
//  Game – klasa bazowa
// ============================================================
void Game::printGameBanner() const {
    UI::clearScreen();
    std::cout << Color::BRIGHT_YELLOW << Color::BOLD;
    UI::printDoubleLine();
    UI::printCentered("*** ROYAL FLUSH CASINO ***", 60, Color::BRIGHT_YELLOW + Color::BOLD);
    UI::printCentered(m_name, 60, Color::BRIGHT_CYAN + Color::BOLD);
    UI::printDoubleLine();
    std::cout << Color::RESET;
    m_player.printHeader();
}

double Game::askForBet(double minBet) const {
    double maxBet = m_player.getBalance();
    if (maxBet < minBet) {
        std::cout << Color::BRIGHT_RED
                  << "\n  Niewystarczajace saldo! Minimum: "
                  << minBet << " zl\n" << Color::RESET;
        return 0.0;
    }
    std::cout << Color::DIM << "  (Min: " << minBet
              << " zl  |  Max: " << maxBet << " zl)\n" << Color::RESET;
    return UI::getDoubleInput("  Twoj zaklad: ", minBet, maxBet);
}

// ============================================================
//  BlackjackGame
// ============================================================
BlackjackGame::BlackjackGame(Player& player)
    : Game(player, "BLACKJACK  [21]"), m_deck(6) // 6 talii jak w kasynie
{}

void BlackjackGame::printTable(bool hideDealer) const {
    std::cout << "\n";
    std::cout << Color::BG_GREEN << Color::BRIGHT_WHITE
              << "                   STOL BLACKJACKA                   "
              << Color::RESET << "\n\n";

    std::cout << Color::BRIGHT_YELLOW << "  Krupier: " << Color::RESET;
    m_dealerHand.print(hideDealer);

    UI::printLine('-', 50, Color::BRIGHT_GREEN);

    std::cout << Color::BRIGHT_CYAN << "  Ty:      " << Color::RESET;
    m_playerHand.print(false);
    std::cout << "\n";
}

bool BlackjackGame::offerInsurance(double bet) {
    if (m_dealerHand.getCards()[1].getRank() != Rank::ACE) return false;
    std::cout << Color::BRIGHT_YELLOW
              << "\n  Krupier pokazuje ASA! Chcesz ubezpieczenie? (max " << bet/2 << " zl)\n"
              << Color::RESET;
    int choice = UI::getIntInput("  [1] Tak  [2] Nie > ", 1, 2);
    if (choice == 1) {
        double ins = bet / 2.0;
        if (m_player.placeBet(ins)) {
            if (m_dealerHand.getValue() == 21) {
                std::cout << Color::BRIGHT_GREEN
                          << "\n  Krupier ma Blackjacka! Ubezpieczenie wyplacone (2:1)!\n"
                          << Color::RESET;
                m_player.addWinnings(ins * 3.0); // ins*2 + zwrot stawki ins
                return true;
            } else {
                std::cout << Color::BRIGHT_RED
                          << "\n  Krupier nie ma Blackjacka. Tracisz ubezpieczenie.\n"
                          << Color::RESET;
            }
        }
    }
    return false;
}

bool BlackjackGame::playerTurn(double bet) {
    while (true) {
        printTable(true);

        if (m_playerHand.isBust()) {
            std::cout << Color::BRIGHT_RED << "\n  BUST! Przekroczyłes 21!\n"
                      << Color::RESET;
            return false;
        }
        if (m_playerHand.isBlackjack() && m_playerHand.size() == 2) {
            return true; // automat do fazy krupiera
        }

        std::cout << Color::BRIGHT_YELLOW
                  << "  Twoja wartosc: " << m_playerHand.getValue() << "\n\n"
                  << Color::RESET;

        std::cout << "  [1] Dobierz   [2] Stoj";
        // Double Down tylko przy pierwszych 2 kartach
        bool canDouble = (m_playerHand.size() == 2 && m_player.getBalance() >= bet);
        if (canDouble) std::cout << "   [3] Podwoj zaklad";
        std::cout << "\n";

        int maxOpt = canDouble ? 3 : 2;
        int choice = UI::getIntInput("  Twoj wybor: ", 1, maxOpt);

        if (choice == 1) {
            m_playerHand.addCard(m_deck.deal());
        } else if (choice == 2) {
            return true;
        } else if (choice == 3 && canDouble) {
            if (m_player.placeBet(bet)) {
                bet *= 2; // bet jest lokalną kopią – nie możemy go tutaj zwrócić
                // Inny mechanizm – tylko jedna karta
                m_playerHand.addCard(m_deck.deal());
                printTable(true);
                std::cout << Color::BRIGHT_MAGENTA
                          << "\n  DOUBLE DOWN! Zaklad podwojony do " << bet << " zl\n"
                          << Color::RESET;
                return true;
            }
        }
    }
}

void BlackjackGame::dealerTurn() {
    printTable(false);
    // Krupier dobiera do 17
    while (m_dealerHand.getValue() < 17) {
        std::cout << Color::DIM << "\n  Krupier dobiera...\n" << Color::RESET;
        m_dealerHand.addCard(m_deck.deal());
        printTable(false);
    }
}

void BlackjackGame::resolveRound(double bet) {
    int pv = m_playerHand.getValue();
    int dv = m_dealerHand.getValue();
    bool pBust = m_playerHand.isBust();
    bool dBust = m_dealerHand.isBust();
    bool pBJ   = m_playerHand.isBlackjack() && m_playerHand.size() == 2;
    bool dBJ   = m_dealerHand.isBlackjack() && m_dealerHand.size() == 2;

    UI::printLine('=', 50, Color::BRIGHT_YELLOW);
    double win = 0.0;
    std::string detail;

    if (pBust) {
        std::cout << Color::BRIGHT_RED << "\n  PRZEGRYWASZ! Bust!\n" << Color::RESET;
        detail = "Bust";
        win = -bet;
    } else if (dBust) {
        std::cout << Color::BRIGHT_GREEN << "\n  WYGRYWASZ! Krupier ma Bust!\n" << Color::RESET;
        m_player.addWinnings(bet * 2.0);
        detail = "Krupier Bust";
        win = bet;
    } else if (pBJ && !dBJ) {
        double payout = bet * 2.5; // 3:2
        std::cout << Color::BRIGHT_GREEN << Color::BOLD
                  << "\n  BLACKJACK! Wygrywasz " << payout << " zl! (3:2)\n"
                  << Color::RESET;
        m_player.addWinnings(payout);
        detail = "Blackjack!";
        win = bet * 1.5;
    } else if (dBJ && !pBJ) {
        std::cout << Color::BRIGHT_RED << "\n  Krupier ma Blackjacka! Przegrywasz.\n" << Color::RESET;
        detail = "Krupier BJ";
        win = -bet;
    } else if (pBJ && dBJ) {
        std::cout << Color::BRIGHT_YELLOW << "\n  REMIS – oboje macie Blackjacka!\n" << Color::RESET;
        m_player.addWinnings(bet);
        detail = "Remis BJ";
        win = 0.0;
    } else if (pv > dv) {
        std::cout << Color::BRIGHT_GREEN << "\n  WYGRYWASZ! " << pv << " vs " << dv << "\n" << Color::RESET;
        m_player.addWinnings(bet * 2.0);
        detail = "Wygrana";
        win = bet;
    } else if (dv > pv) {
        std::cout << Color::BRIGHT_RED << "\n  PRZEGRYWASZ! " << pv << " vs " << dv << "\n" << Color::RESET;
        detail = "Przegrana";
        win = -bet;
    } else {
        std::cout << Color::BRIGHT_YELLOW << "\n  REMIS! " << pv << " vs " << dv << "\n" << Color::RESET;
        m_player.addWinnings(bet);
        detail = "Remis";
        win = 0.0;
    }

    std::cout << Color::BRIGHT_CYAN << "  Saldo po rundzie: "
              << Color::BRIGHT_WHITE << std::fixed << std::setprecision(2)
              << m_player.getBalance() << " zl\n" << Color::RESET;
    m_player.recordResult("Blackjack", bet, win, detail);
    UI::printLine('=', 50, Color::BRIGHT_YELLOW);
}

void BlackjackGame::run() {
    while (true) {
        printGameBanner();

        std::cout << Color::BRIGHT_CYAN
                  << "  Zasady: dobierz do 21, nie przekraczaj.\n"
                  << "  Blackjack placi 3:2. Krupier staje na 17.\n\n"
                  << Color::RESET;

        if (m_player.getBalance() < 1.0) {
            std::cout << Color::BRIGHT_RED
                      << "\n  Brak srodkow! Wróc do kasy.\n" << Color::RESET;
            UI::pressEnter();
            return;
        }

        double bet = askForBet(1.0);
        if (bet <= 0) { UI::pressEnter(); return; }

        m_player.placeBet(bet);

        // Resetuj ręce
        m_playerHand.clear();
        m_dealerHand.clear();

        // Rozdaj po 2 karty
        m_playerHand.addCard(m_deck.deal());
        m_dealerHand.addCard(m_deck.deal());
        m_playerHand.addCard(m_deck.deal());
        m_dealerHand.addCard(m_deck.deal()); // druga karta krupiera – widoczna

        // Sprawdź ubezpieczenie
        bool insWon = offerInsurance(bet);

        if (!insWon) {
            // Tura gracza
            playerTurn(bet);
            if (!m_playerHand.isBust()) {
                dealerTurn();
            }
            printTable(false);
            resolveRound(bet);
        }

        if (m_deck.needsReshuffle()) {
            std::cout << Color::DIM << "\n  Tasowanie talii...\n" << Color::RESET;
            m_deck = Deck(6);
        }

        UI::pressEnter();

        std::cout << "\n  [1] Graj dalej  [2] Wróc do lobby\n";
        int c = UI::getIntInput("  > ", 1, 2);
        if (c == 2) return;
    }
}
