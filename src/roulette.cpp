#include "../include/game.h"

// ============================================================
//  RouletteGame
// ============================================================
const int RouletteGame::RED_NUMBERS[] = {
    1,3,5,7,9,12,14,16,18,19,21,23,25,27,30,32,34,36
};

RouletteGame::RouletteGame(Player& player)
    : Game(player, "RULETKA EUROPEJSKA")
{}

int RouletteGame::spin() const {
    return rand() % 37; // 0..36
}

bool RouletteGame::isRed(int n) const {
    if (n == 0) return false;
    for (int r : RED_NUMBERS)
        if (r == n) return true;
    return false;
}

void RouletteGame::printWheel(int result) const {
    std::cout << "\n";
    std::cout << Color::BRIGHT_YELLOW
              << "  ╔══════════════════════════════════╗\n"
              << "  ║        KOLO RULETKI OBRACA...    ║\n"
              << "  ╠══════════════════════════════════╣\n"
              << "  ║   WYNIK:  ";

    if (result == 0) {
        std::cout << Color::BRIGHT_GREEN << Color::BOLD
                  << "  0  (ZERO) ";
    } else if (isRed(result)) {
        std::cout << Color::BRIGHT_RED << Color::BOLD
                  << std::setw(3) << result << " (CZERWONY)";
    } else {
        std::cout << Color::DIM << Color::BOLD
                  << std::setw(3) << result << " (CZARNY)  ";
    }
    std::cout << Color::BRIGHT_YELLOW
              << "   ║\n"
              << "  ╚══════════════════════════════════╝\n"
              << Color::RESET << "\n";
}

void RouletteGame::printBettingMenu() const {
    std::cout << Color::BRIGHT_CYAN
              << "\n  RODZAJE ZAKLADOW:\n"
              << Color::RESET
              << "  [1]  Pojedyncza liczba (0-36)   -  35:1\n"
              << "  [2]  Czerwony                   -   1:1\n"
              << "  [3]  Czarny                     -   1:1\n"
              << "  [4]  Parzyste (2-36)             -   1:1\n"
              << "  [5]  Nieparzyste (1-35)          -   1:1\n"
              << "  [6]  Niskie (1-18)               -   1:1\n"
              << "  [7]  Wysokie (19-36)             -   1:1\n"
              << "  [8]  Tuzin 1 (1-12)              -   2:1\n"
              << "  [9]  Tuzin 2 (13-24)             -   2:1\n"
              << " [10]  Tuzin 3 (25-36)             -   2:1\n"
              << " [11]  Kolumna 1                   -   2:1\n"
              << " [12]  Kolumna 2                   -   2:1\n"
              << " [13]  Kolumna 3                   -   2:1\n"
              << " [14]  Zakoncz ture\n\n";
}

double RouletteGame::getPayoutMultiplier(const RouletteBet& bet, int result) const {
    using T = RouletteBet::Type;
    switch (bet.type) {
        case T::STRAIGHT:
            return (result == bet.number) ? 35.0 : -1.0;
        case T::RED:
            return isRed(result) ? 1.0 : -1.0;
        case T::BLACK:
            return (!isRed(result) && result != 0) ? 1.0 : -1.0;
        case T::EVEN:
            return (result != 0 && result % 2 == 0) ? 1.0 : -1.0;
        case T::ODD:
            return (result % 2 == 1) ? 1.0 : -1.0;
        case T::LOW:
            return (result >= 1 && result <= 18) ? 1.0 : -1.0;
        case T::HIGH:
            return (result >= 19 && result <= 36) ? 1.0 : -1.0;
        case T::DOZEN_1:
            return (result >= 1 && result <= 12) ? 2.0 : -1.0;
        case T::DOZEN_2:
            return (result >= 13 && result <= 24) ? 2.0 : -1.0;
        case T::DOZEN_3:
            return (result >= 25 && result <= 36) ? 2.0 : -1.0;
        case T::COLUMN_1:
            return (result != 0 && result % 3 == 1) ? 2.0 : -1.0;
        case T::COLUMN_2:
            return (result != 0 && result % 3 == 2) ? 2.0 : -1.0;
        case T::COLUMN_3:
            return (result != 0 && result % 3 == 0) ? 2.0 : -1.0;
        default: return -1.0;
    }
}

double RouletteGame::resolveAllBets(const std::vector<RouletteBet>& bets, int result) const {
    double net = 0.0;
    std::cout << "\n" << Color::BRIGHT_YELLOW << "  ROZLICZENIE:\n" << Color::RESET;
    for (const auto& bet : bets) {
        double mult = getPayoutMultiplier(bet, result);
        double gain = bet.amount * mult;
        net += gain;
        if (mult > 0) {
            m_player.addWinnings(bet.amount + gain);
            std::cout << Color::BRIGHT_GREEN
                      << "  TRAFIONY! +" << gain << " zl (zaklad: " << bet.amount << " zl)\n"
                      << Color::RESET;
        } else {
            std::cout << Color::BRIGHT_RED
                      << "  CHYBIONY  -" << bet.amount << " zl\n"
                      << Color::RESET;
        }
    }
    return net;
}

void RouletteGame::run() {
    while (true) {
        printGameBanner();
        std::cout << Color::BRIGHT_CYAN
                  << "  Mozesz postawic wiele zakladow na ture!\n\n"
                  << Color::RESET;

        if (m_player.getBalance() < 1.0) {
            std::cout << Color::BRIGHT_RED
                      << "\n  Brak srodkow!\n" << Color::RESET;
            UI::pressEnter(); return;
        }

        std::vector<RouletteBet> bets;
        double totalBet = 0.0;

        // Faza zbierania zakładów
        while (true) {
            printBettingMenu();
            if (!bets.empty()) {
                std::cout << Color::DIM << "  Dotychczasowe zaklady: " << totalBet
                          << " zl (" << bets.size() << " zakladow)\n" << Color::RESET;
            }
            int opt = UI::getIntInput("  Wybor: ", 1, 14);
            if (opt == 14) {
                if (bets.empty()) {
                    std::cout << Color::BRIGHT_RED << "  Musisz postawic przynajmniej jeden zaklad!\n"
                              << Color::RESET;
                    continue;
                }
                break;
            }

            double maxNow = m_player.getBalance() - totalBet;
            if (maxNow < 1.0) {
                std::cout << Color::BRIGHT_RED << "  Brak srodkow na kolejny zaklad!\n"
                          << Color::RESET;
                break;
            }
            std::cout << Color::DIM << "  (Max: " << maxNow << " zl)\n" << Color::RESET;
            double amt = UI::getDoubleInput("  Kwota zakladu: ", 1.0, maxNow);

            RouletteBet rb;
            rb.amount = amt;
            totalBet += amt;
            m_player.placeBet(amt);

            using T = RouletteBet::Type;
            switch (opt) {
                case 1:
                    rb.type = T::STRAIGHT;
                    rb.number = UI::getIntInput("  Wybierz liczbe (0-36): ", 0, 36);
                    break;
                case 2:  rb.type = T::RED;      break;
                case 3:  rb.type = T::BLACK;     break;
                case 4:  rb.type = T::EVEN;      break;
                case 5:  rb.type = T::ODD;       break;
                case 6:  rb.type = T::LOW;       break;
                case 7:  rb.type = T::HIGH;      break;
                case 8:  rb.type = T::DOZEN_1;   break;
                case 9:  rb.type = T::DOZEN_2;   break;
                case 10: rb.type = T::DOZEN_3;   break;
                case 11: rb.type = T::COLUMN_1;  break;
                case 12: rb.type = T::COLUMN_2;  break;
                case 13: rb.type = T::COLUMN_3;  break;
            }
            bets.push_back(rb);
            std::cout << Color::BRIGHT_GREEN << "  Zaklad dodany!\n" << Color::RESET;
        }

        // Kręć kołem
        std::cout << "\n" << Color::BRIGHT_YELLOW
                  << "  Nacisnij ENTER aby zakrecic kolowrotem..." << Color::RESET;
        std::cin.get();

        int result = spin();
        printWheel(result);

        double net = resolveAllBets(bets, result);

        std::cout << "\n" << Color::BRIGHT_CYAN
                  << "  Wynik netto: ";
        if (net >= 0)
            std::cout << Color::BRIGHT_GREEN << "+" << net;
        else
            std::cout << Color::BRIGHT_RED << net;
        std::cout << " zl\n"
                  << Color::BRIGHT_CYAN << "  Saldo: "
                  << Color::BRIGHT_WHITE << m_player.getBalance() << " zl\n"
                  << Color::RESET;

        m_player.recordResult("Ruletka", totalBet, net,
                              "Wynik: " + std::to_string(result));

        UI::pressEnter();
        std::cout << "\n  [1] Graj dalej  [2] Wróc do lobby\n";
        int c = UI::getIntInput("  > ", 1, 2);
        if (c == 2) return;
    }
}
