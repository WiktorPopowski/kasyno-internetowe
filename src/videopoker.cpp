#include "../include/game.h"

// ============================================================
//  VideoPoker – Jacks or Better
// ============================================================
VideoPoker::VideoPoker(Player& player)
    : Game(player, "VIDEO POKER  [Jacks or Better]"), m_deck(1)
{}

bool VideoPoker::isStraight() const {
    std::vector<int> vals;
    for (const auto& c : m_hand) {
        int r = (int)c.getRank();
        vals.push_back(r);
    }
    std::sort(vals.begin(), vals.end());
    // As może być na początku (A-2-3-4-5)
    if (vals.back() == 14) { // Ace
        // Sprawdź A-low: A,2,3,4,5
        std::vector<int> lowAce = {1,2,3,4,5};
        std::vector<int> check = vals;
        check[4] = 1;
        std::sort(check.begin(), check.end());
        if (check == lowAce) return true;
    }
    for (int i = 1; i < 5; ++i)
        if (vals[i] != vals[i-1] + 1) return false;
    return true;
}

bool VideoPoker::isFlush() const {
    Suit s = m_hand[0].getSuit();
    for (const auto& c : m_hand)
        if (c.getSuit() != s) return false;
    return true;
}

bool VideoPoker::isRoyalFlush() const {
    if (!isFlush()) return false;
    std::vector<int> vals;
    for (const auto& c : m_hand) vals.push_back((int)c.getRank());
    std::sort(vals.begin(), vals.end());
    std::vector<int> royal = {10,11,12,13,14};
    return vals == royal;
}

VideoPoker::HandRank VideoPoker::evaluateHand() const {
    // Policz wystąpienia wartości
    std::map<int,int> freq;
    for (const auto& c : m_hand)
        freq[(int)c.getRank()]++;

    std::vector<int> counts;
    for (auto& p : freq) counts.push_back(p.second);
    std::sort(counts.rbegin(), counts.rend());

    bool flush    = isFlush();
    bool straight = isStraight();
    bool royal    = isRoyalFlush();

    if (royal)                                      return HandRank::ROYAL_FLUSH;
    if (flush && straight)                          return HandRank::STRAIGHT_FLUSH;
    if (counts[0] == 4)                             return HandRank::FOUR_OF_A_KIND;
    if (counts[0] == 3 && counts[1] == 2)          return HandRank::FULL_HOUSE;
    if (flush)                                      return HandRank::FLUSH;
    if (straight)                                   return HandRank::STRAIGHT;
    if (counts[0] == 3)                             return HandRank::THREE_OF_A_KIND;
    if (counts[0] == 2 && counts[1] == 2)          return HandRank::TWO_PAIR;

    // Para – tylko J,Q,K,A (Jacks or Better)
    if (counts[0] == 2) {
        for (auto& p : freq) {
            if (p.second == 2 && p.first >= 11) // J=11
                return HandRank::ONE_PAIR;
        }
    }
    return HandRank::HIGH_CARD;
}

std::string VideoPoker::handRankName(HandRank r) const {
    switch(r) {
        case HandRank::ROYAL_FLUSH:     return "ROYAL FLUSH!!!";
        case HandRank::STRAIGHT_FLUSH:  return "Straight Flush";
        case HandRank::FOUR_OF_A_KIND:  return "Kareta (4 takie same)";
        case HandRank::FULL_HOUSE:      return "Full House";
        case HandRank::FLUSH:           return "Kolor (Flush)";
        case HandRank::STRAIGHT:        return "Strit (Straight)";
        case HandRank::THREE_OF_A_KIND: return "Trojka";
        case HandRank::TWO_PAIR:        return "Dwie pary";
        case HandRank::ONE_PAIR:        return "Para (J lub wyzej)";
        case HandRank::HIGH_CARD:       return "Brak ukladu";
        default:                        return "???";
    }
}

int VideoPoker::handRankPayout(HandRank r) const {
    // Standardowe wyplaty Jacks or Better
    switch(r) {
        case HandRank::ROYAL_FLUSH:     return 800;
        case HandRank::STRAIGHT_FLUSH:  return 50;
        case HandRank::FOUR_OF_A_KIND:  return 25;
        case HandRank::FULL_HOUSE:      return 9;
        case HandRank::FLUSH:           return 6;
        case HandRank::STRAIGHT:        return 4;
        case HandRank::THREE_OF_A_KIND: return 3;
        case HandRank::TWO_PAIR:        return 2;
        case HandRank::ONE_PAIR:        return 1;
        case HandRank::HIGH_CARD:       return 0;
        default:                        return 0;
    }
}

void VideoPoker::printHand(const std::vector<bool>& held) const {
    std::cout << "\n  ";
    for (int i = 0; i < (int)m_hand.size(); ++i) {
        m_hand[i].print();
        std::cout << " ";
    }
    std::cout << "\n  ";
    for (int i = 0; i < (int)m_hand.size(); ++i) {
        if (held[i])
            std::cout << Color::BRIGHT_GREEN << "[TRZYMAJ]" << Color::RESET;
        else
            std::cout << Color::DIM << " [  --  ] " << Color::RESET;
        std::cout << " ";
    }
    std::cout << "\n  Karta: ";
    for (int i = 0; i < (int)m_hand.size(); ++i)
        std::cout << "  [" << (i+1) << "]    ";
    std::cout << "\n\n";
}

void VideoPoker::printPaytable() const {
    std::cout << Color::DIM
              << "  Royal Flush: 800x | Str.Flush: 50x | Kareta: 25x\n"
              << "  Full House: 9x | Kolor: 6x | Strit: 4x\n"
              << "  Trojka: 3x | Dwie pary: 2x | Para (J+): 1x\n"
              << Color::RESET;
    UI::printLine('-', 55, Color::BRIGHT_CYAN);
}

void VideoPoker::run() {
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

        // Rozdaj 5 kart
        if (m_deck.remaining() < 10) m_deck = Deck(1);
        m_hand.clear();
        for (int i = 0; i < 5; ++i)
            m_hand.push_back(m_deck.deal());

        std::vector<bool> held(5, false);

        // Wyświetl rękę i zapytaj co trzymać
        std::cout << "\n" << Color::BRIGHT_YELLOW
                  << "  TWOJA REKA:\n" << Color::RESET;
        printHand(held);

        std::cout << Color::BRIGHT_CYAN
                  << "  Wybierz karty do zatrzymania (1-5, oddzielone spacjami).\n"
                  << "  0 = zmien wszystkie, 6 = zatrzymaj wszystkie\n"
                  << Color::RESET;

        std::string line;
        std::cout << "  Twoj wybor: ";
        std::getline(std::cin, line);

        if (line == "6") {
            for (int i = 0; i < 5; ++i) held[i] = true;
        } else if (line != "0") {
            std::istringstream ss(line);
            int n;
            while (ss >> n)
                if (n >= 1 && n <= 5) held[n-1] = true;
        }

        // Wymień niezatrzymane karty
        for (int i = 0; i < 5; ++i)
            if (!held[i]) m_hand[i] = m_deck.deal();

        std::cout << "\n" << Color::BRIGHT_YELLOW
                  << "  OSTATECZNA REKA:\n" << Color::RESET;
        std::vector<bool> allHeld(5, true);
        printHand(allHeld);

        HandRank rank = evaluateHand();
        int mult = handRankPayout(rank);
        double win = bet * mult;

        if (mult > 0) {
            std::cout << Color::BRIGHT_GREEN << Color::BOLD
                      << "  >>> " << handRankName(rank)
                      << " <<< Wygrywasz " << win << " zl! (" << mult << "x)\n"
                      << Color::RESET;
            m_player.addWinnings(win);
        } else {
            std::cout << Color::BRIGHT_RED
                      << "  " << handRankName(rank) << " - Brak wygranej.\n"
                      << Color::RESET;
        }

        std::cout << Color::BRIGHT_CYAN << "  Saldo: "
                  << Color::BRIGHT_WHITE
                  << std::fixed << std::setprecision(2)
                  << m_player.getBalance() << " zl\n" << Color::RESET;

        double net = win - bet;
        m_player.recordResult("Video Poker", bet, net, handRankName(rank));

        UI::pressEnter();
        std::cout << "\n  [1] Graj dalej  [2] Wróc do lobby\n";
        int c = UI::getIntInput("  > ", 1, 2);
        if (c == 2) return;
    }
}
