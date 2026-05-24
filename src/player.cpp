#include "../include/player.h"

// ============================================================
//  Konstruktor
// ============================================================
Player::Player(const std::string& name, double startBalance)
    : m_name(name), m_balance(startBalance),
      m_totalWon(0.0), m_totalLost(0.0), m_gamesPlayed(0)
{}

// ============================================================
//  Operacje na saldzie
// ============================================================
bool Player::placeBet(double amount) {
    if (amount <= 0 || amount > m_balance) return false;
    m_balance -= amount;
    return true;
}

void Player::addWinnings(double amount) {
    m_balance += amount;
}

void Player::recordResult(const std::string& game, double bet,
                          double resultDelta, const std::string& detail) {
    ++m_gamesPlayed;
    if (resultDelta > 0) m_totalWon  += resultDelta;
    else                 m_totalLost += (-resultDelta);

    m_history.emplace_back(game, bet, resultDelta, detail);
    if ((int)m_history.size() > MAX_HISTORY)
        m_history.erase(m_history.begin());
}

void Player::deposit(double amount) {
    if (amount > 0) m_balance += amount;
}

bool Player::withdraw(double amount) {
    if (amount > 0 && amount <= m_balance) {
        m_balance -= amount;
        return true;
    }
    return false;
}

// ============================================================
//  Wyświetlanie
// ============================================================
void Player::printHeader() const {
    std::cout << "\n";
    std::cout << Color::BG_BLUE << Color::BRIGHT_WHITE
              << "  Gracz: " << m_name
              << "   Saldo: " << std::fixed << std::setprecision(2)
              << m_balance << " zl  "
              << Color::RESET << "\n\n";
}

void Player::printStats() const {
    UI::printDoubleLine();
    UI::printCentered("STATYSTYKI GRACZA", 60, Color::BRIGHT_YELLOW);
    UI::printDoubleLine();
    std::cout << Color::BRIGHT_CYAN
              << "  Gracz         : " << Color::BRIGHT_WHITE << m_name << "\n"
              << Color::BRIGHT_CYAN
              << "  Saldo         : " << Color::BRIGHT_GREEN
              << std::fixed << std::setprecision(2) << m_balance << " zl\n"
              << Color::BRIGHT_CYAN
              << "  Rozegrane gry : " << Color::BRIGHT_WHITE << m_gamesPlayed << "\n"
              << Color::BRIGHT_CYAN
              << "  Laczna wygrana: " << Color::BRIGHT_GREEN
              << m_totalWon << " zl\n"
              << Color::BRIGHT_CYAN
              << "  Laczna strata : " << Color::BRIGHT_RED
              << m_totalLost << " zl\n"
              << Color::BRIGHT_CYAN
              << "  Bilans netto  : ";
    double net = getNetProfit();
    if (net >= 0)
        std::cout << Color::BRIGHT_GREEN << "+" << net << " zl\n";
    else
        std::cout << Color::BRIGHT_RED << net << " zl\n";
    std::cout << Color::RESET;
    UI::printDoubleLine();
}

void Player::printHistory() const {
    UI::printDoubleLine();
    UI::printCentered("HISTORIA OSTATNICH GIR", 60, Color::BRIGHT_YELLOW);
    UI::printDoubleLine();
    if (m_history.empty()) {
        UI::printCentered("Brak historii.", 60, Color::DIM);
    } else {
        // Pokaż od najnowszych
        int start = std::max(0, (int)m_history.size() - 15);
        for (int i = (int)m_history.size() - 1; i >= start; --i) {
            const auto& rec = m_history[i];
            std::cout << Color::BRIGHT_CYAN << "  "
                      << std::setw(12) << std::left << rec.game
                      << Color::WHITE << "  Zaklad: "
                      << std::setw(8) << std::right
                      << std::fixed << std::setprecision(2) << rec.bet
                      << "  Wynik: ";
            if (rec.result >= 0)
                std::cout << Color::BRIGHT_GREEN << "+" << rec.result;
            else
                std::cout << Color::BRIGHT_RED << rec.result;
            std::cout << Color::DIM << "  " << rec.detail << Color::RESET << "\n";
        }
    }
    UI::printDoubleLine();
}

// ============================================================
//  Zapis / Odczyt pliku
// ============================================================
bool Player::saveToFile() const {
    std::string filename = m_name + ".casino";
    std::ofstream f(filename);
    if (!f.is_open()) return false;
    f << m_name << "\n"
      << std::fixed << std::setprecision(2)
      << m_balance   << "\n"
      << m_totalWon  << "\n"
      << m_totalLost << "\n"
      << m_gamesPlayed << "\n";
    // Historia
    f << m_history.size() << "\n";
    for (const auto& rec : m_history) {
        f << rec.game << "|" << rec.bet << "|"
          << rec.result << "|" << rec.detail << "\n";
    }
    return true;
}

bool Player::loadFromFile(const std::string& name) {
    std::string filename = name + ".casino";
    std::ifstream f(filename);
    if (!f.is_open()) return false;
    std::string n;
    std::getline(f, n);
    if (n != name) return false;
    f >> m_balance >> m_totalWon >> m_totalLost >> m_gamesPlayed;
    f.ignore();
    int histSize;
    f >> histSize;
    f.ignore();
    m_history.clear();
    for (int i = 0; i < histSize; ++i) {
        std::string line;
        if (!std::getline(f, line)) break;
        // Parse "game|bet|result|detail"
        std::istringstream ss(line);
        std::string g, detail;
        double b, r;
        char sep;
        std::getline(ss, g, '|');
        ss >> b >> sep >> r;
        ss.ignore();
        std::getline(ss, detail);
        m_history.emplace_back(g, b, r, detail);
    }
    m_name = n;
    return true;
}
