#include "../include/game.h"

// ============================================================
//  SlotMachine – statyczne dane
// ============================================================
const std::vector<SlotMachine::Symbol> SlotMachine::SYMBOLS = {
    // display, name, weight, pay3, pay4, pay5
    { "\xf0\x9f\x8d\x92", "Sliwka",    30,  5,  10,  50  },  // 🍒
    { "\xf0\x9f\x8d\x8b", "Cytryna",   25,  8,  15,  75  },  // 🍋
    { "\xf0\x9f\x8d\x8a", "Pomarancza",20, 10,  20, 100  },  // 🍊
    { "\xf0\x9f\x8d\x87", "Winogrono", 15, 15,  30, 150  },  // 🍇
    { "\xf0\x9f\x94\x94", "Dzwonek",   10, 25,  50, 250  },  // 🔔
    { "\xf0\x9f\x92\x8e", "Diament",    5, 50, 100, 500  },  // 💎
    { "\xf0\x9f\x8f\x86", "Puchar",     3, 75, 150, 750  },  // 🏆
    { "\x37\xef\xb8\x8f\xe2\x83\xa3", "Siodemka",  2,100, 200,1000  },  // 7️⃣
};

SlotMachine::SlotMachine(Player& player)
    : Game(player, "SLOT MACHINE  [5 BEBNY]")
{
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < REELS; ++c)
            m_grid[r][c] = 0;
}

int SlotMachine::spinSymbol() const {
    // Ważone losowanie
    int totalWeight = 0;
    for (const auto& s : SYMBOLS) totalWeight += s.weight;
    int roll = rand() % totalWeight;
    int acc  = 0;
    for (int i = 0; i < (int)SYMBOLS.size(); ++i) {
        acc += SYMBOLS[i].weight;
        if (roll < acc) return i;
    }
    return 0;
}

void SlotMachine::spinAllReels() {
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < REELS; ++c)
            m_grid[r][c] = spinSymbol();
}

void SlotMachine::printGrid() const {
    std::cout << "\n";
    std::cout << Color::BRIGHT_YELLOW
              << "  +-------+-------+-------+-------+-------+\n"
              << Color::RESET;
    for (int r = 0; r < ROWS; ++r) {
        std::string lineColor = (r == 1)
            ? (Color::BRIGHT_GREEN + Color::BOLD)
            : Color::DIM;

        // Symbole (użyj ASCII fallback jeśli emoji nie działa)
        std::cout << Color::BRIGHT_YELLOW << "  |";
        for (int c = 0; c < REELS; ++c) {
            // Skróć symbol do 3 znaków ASCII dla kompatybilności
            const std::string& sym = SYMBOLS[m_grid[r][c]].name;
            // Weź pierwsze 7 liter nazwy
            std::string shortName = sym.substr(0, 7);
            std::cout << lineColor
                      << std::setw(7) << std::left << shortName
                      << Color::BRIGHT_YELLOW << "|";
        }
        if (r == 1)
            std::cout << Color::BRIGHT_GREEN << " <-- LINIA WYPLAT";
        std::cout << Color::RESET << "\n";
        std::cout << Color::BRIGHT_YELLOW
                  << "  +-------+-------+-------+-------+-------+\n"
                  << Color::RESET;
    }
    std::cout << "\n";
}

double SlotMachine::evaluateWin(double bet) const {
    double totalWin = 0.0;
    // Sprawdź środkowy wiersz (główna linia)
    int lineRow = 1;
    int sym0 = m_grid[lineRow][0];
    int count = 1;
    for (int c = 1; c < REELS; ++c) {
        if (m_grid[lineRow][c] == sym0) ++count;
        else break;
    }

    if (count >= 3) {
        const Symbol& s = SYMBOLS[sym0];
        int payout = 0;
        if      (count == 3) payout = s.payoutLine3;
        else if (count == 4) payout = s.payoutLine4;
        else if (count == 5) payout = s.payoutLine5;

        double win = bet * payout;
        totalWin += win;

        std::cout << Color::BRIGHT_GREEN << Color::BOLD
                  << "\n  TRAFIENIE! " << count << "x " << s.name
                  << " -> +" << win << " zl  (" << payout << "x zaklad)\n"
                  << Color::RESET;
    }
    return totalWin;
}

void SlotMachine::printPaytable() const {
    UI::printLine('-', 55, Color::BRIGHT_YELLOW);
    std::cout << Color::BRIGHT_YELLOW << "  TABELA WYPLAT (mnoznik x zaklad):\n" << Color::RESET;
    std::cout << Color::DIM
              << "  Symbol      | 3x  | 4x  | 5x\n"
              << "  ------------|-----|-----|------\n";
    for (const auto& s : SYMBOLS) {
        std::cout << "  " << std::setw(12) << std::left << s.name
                  << "| " << std::setw(4) << s.payoutLine3
                  << "| " << std::setw(4) << s.payoutLine4
                  << "| " << s.payoutLine5 << "\n";
    }
    std::cout << Color::RESET;
    UI::printLine('-', 55, Color::BRIGHT_YELLOW);
}

void SlotMachine::run() {
    while (true) {
        printGameBanner();
        printPaytable();

        if (m_player.getBalance() < 1.0) {
            std::cout << Color::BRIGHT_RED << "\n  Brak srodkow!\n" << Color::RESET;
            UI::pressEnter(); return;
        }

        double bet = askForBet(1.0);
        if (bet <= 0) { UI::pressEnter(); return; }

        m_player.placeBet(bet);

        std::cout << "\n" << Color::BRIGHT_YELLOW
                  << "  Krecenie bebnami...\n" << Color::RESET;

        spinAllReels();
        printGrid();

        double win = evaluateWin(bet);

        if (win > 0) {
            m_player.addWinnings(win);
            std::cout << Color::BRIGHT_GREEN
                      << "  Laczna wygrana: +" << win << " zl\n" << Color::RESET;
        } else {
            std::cout << Color::BRIGHT_RED
                      << "\n  Brak wygranej. Sprobuj ponownie!\n" << Color::RESET;
        }

        std::cout << Color::BRIGHT_CYAN << "  Saldo: "
                  << Color::BRIGHT_WHITE << std::fixed << std::setprecision(2)
                  << m_player.getBalance() << " zl\n" << Color::RESET;

        double netResult = win - bet;
        m_player.recordResult("Slot Machine", bet, netResult,
                              win > 0 ? "Wygrana " + std::to_string((int)win) : "Chybienie");

        UI::pressEnter();
        std::cout << "\n  [1] Graj dalej  [2] Wróc do lobby\n";
        int c = UI::getIntInput("  > ", 1, 2);
        if (c == 2) return;
    }
}
