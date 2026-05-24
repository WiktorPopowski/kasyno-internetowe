#include "../include/game.h"

// ============================================================
//  DiceGame – uproszczone Craps
// ============================================================
DiceGame::DiceGame(Player& player)
    : Game(player, "GRA W KOSCI  [Craps]")
{}

int DiceGame::rollDie() const {
    return (rand() % 6) + 1;
}

int DiceGame::rollTwo() const {
    return rollDie() + rollDie();
}

void DiceGame::printDice(int d1, int d2) const {
    std::cout << "\n";
    // Wyświetl kostki w ASCII art
    auto face = [](int n) -> std::vector<std::string> {
        switch(n) {
            case 1: return {"+-------+","| . |","| . |","| . |","+-------+"};
            case 2: return {"+-------+","|o     |","|      |","|     o|","+-------+"};
            case 3: return {"+-------+","|o     |","|  o   |","|     o|","+-------+"};
            case 4: return {"+-------+","|o   o |","|      |","|o   o |","+-------+"};
            case 5: return {"+-------+","|o   o |","|  o   |","|o   o |","+-------+"};
            case 6: return {"+-------+","|o   o |","|o   o |","|o   o |","+-------+"};
            default: return {"+-------+","| ? |","| ? |","| ? |","+-------+"};
        }
    };

    auto f1 = face(d1), f2 = face(d2);
    std::cout << Color::BRIGHT_WHITE;
    for (int i = 0; i < 5; ++i)
        std::cout << "  " << f1[i] << "   " << f2[i] << "\n";
    std::cout << Color::RESET;
    std::cout << Color::BRIGHT_YELLOW
              << "      [" << d1 << "]           [" << d2 << "]\n"
              << "  SUMA: " << Color::BRIGHT_CYAN << Color::BOLD
              << d1 + d2 << Color::RESET << "\n\n";
}

void DiceGame::printRules() const {
    std::cout << Color::DIM
              << "  ZASADY CRAPS:\n"
              << "  Come-out roll: 7 lub 11 = WYGRANA natychmiastowa\n"
              << "                 2, 3, 12  = PRZEGRANA natychmiastowa\n"
              << "  Inna liczba = ustanawia POINT.\n"
              << "  Kolejne rzuty: trafisz POINT = WYGRANA\n"
              << "                 wyrzucisz 7   = PRZEGRANA\n\n"
              << Color::RESET;
}

void DiceGame::run() {
    while (true) {
        printGameBanner();
        printRules();

        if (m_player.getBalance() < 1.0) {
            std::cout << Color::BRIGHT_RED << "\n  Brak srodkow!\n" << Color::RESET;
            UI::pressEnter(); return;
        }

        double bet = askForBet(1.0);
        if (bet <= 0) { UI::pressEnter(); return; }
        m_player.placeBet(bet);

        // Come-out roll
        std::cout << Color::BRIGHT_YELLOW
                  << "\n  Nacisnij ENTER aby rzucic kostkami (come-out roll)..."
                  << Color::RESET;
        std::cin.get();

        int d1 = rollDie(), d2 = rollDie();
        int sum = d1 + d2;
        printDice(d1, d2);

        bool roundOver = false;
        double win = 0.0;
        std::string detail;
        int point = 0;

        if (sum == 7 || sum == 11) {
            std::cout << Color::BRIGHT_GREEN << Color::BOLD
                      << "  NATURAL! WYGRYWASZ! +" << bet << " zl\n"
                      << Color::RESET;
            m_player.addWinnings(bet * 2.0);
            win = bet;
            detail = "Natural " + std::to_string(sum);
            roundOver = true;
        } else if (sum == 2 || sum == 3 || sum == 12) {
            std::cout << Color::BRIGHT_RED << Color::BOLD
                      << "  CRAPS! PRZEGRYWASZ!\n" << Color::RESET;
            win = -bet;
            detail = "Craps " + std::to_string(sum);
            roundOver = true;
        } else {
            point = sum;
            std::cout << Color::BRIGHT_MAGENTA
                      << "  POINT ustanowiony: " << point << "\n"
                      << "  Rzucaj az trafisz " << point
                      << " (wygrana) lub 7 (przegrana)!\n"
                      << Color::RESET;
        }

        // Runda z point
        while (!roundOver) {
            std::cout << Color::BRIGHT_YELLOW
                      << "\n  Nacisnij ENTER aby rzucic (Point: "
                      << point << ")..." << Color::RESET;
            std::cin.get();

            d1 = rollDie(); d2 = rollDie();
            sum = d1 + d2;
            printDice(d1, d2);

            if (sum == point) {
                std::cout << Color::BRIGHT_GREEN << Color::BOLD
                          << "  TRAFIONY POINT! WYGRYWASZ! +" << bet << " zl\n"
                          << Color::RESET;
                m_player.addWinnings(bet * 2.0);
                win = bet;
                detail = "Point " + std::to_string(point);
                roundOver = true;
            } else if (sum == 7) {
                std::cout << Color::BRIGHT_RED << Color::BOLD
                          << "  SIODEMKA! PRZEGRYWASZ!\n" << Color::RESET;
                win = -bet;
                detail = "Seven-out";
                roundOver = true;
            } else {
                std::cout << Color::DIM
                          << "  Wynik " << sum << " – kontynuuj...\n"
                          << Color::RESET;
            }
        }

        std::cout << Color::BRIGHT_CYAN << "  Saldo: "
                  << Color::BRIGHT_WHITE
                  << std::fixed << std::setprecision(2)
                  << m_player.getBalance() << " zl\n" << Color::RESET;

        m_player.recordResult("Craps/Kosci", bet, win, detail);

        UI::pressEnter();
        std::cout << "\n  [1] Graj dalej  [2] Wróc do lobby\n";
        int c = UI::getIntInput("  > ", 1, 2);
        if (c == 2) return;
    }
}
