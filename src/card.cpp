#include "../include/card.h"

// ============================================================
//  Card – implementacja
// ============================================================
int Card::getBlackjackValue() const {
    int r = (int)m_rank;
    if (r >= 10) return 10; // J, Q, K
    return r;               // 2..9, ACE=1 (specjalnie w BlackjackHand)
}

std::string Card::getRankStr() const {
    switch (m_rank) {
        case Rank::TWO:   return " 2";
        case Rank::THREE: return " 3";
        case Rank::FOUR:  return " 4";
        case Rank::FIVE:  return " 5";
        case Rank::SIX:   return " 6";
        case Rank::SEVEN: return " 7";
        case Rank::EIGHT: return " 8";
        case Rank::NINE:  return " 9";
        case Rank::TEN:   return "10";
        case Rank::JACK:  return " J";
        case Rank::QUEEN: return " Q";
        case Rank::KING:  return " K";
        case Rank::ACE:   return " A";
        default:          return " ?";
    }
}

std::string Card::getSuitStr() const {
    switch (m_suit) {
        case Suit::HEARTS:   return "\xe2\x99\xa5"; // ♥
        case Suit::DIAMONDS: return "\xe2\x99\xa6"; // ♦
        case Suit::CLUBS:    return "\xe2\x99\xa3"; // ♣
        case Suit::SPADES:   return "\xe2\x99\xa0"; // ♠
        default:             return "?";
    }
}

std::string Card::getSuitColor() const {
    if (m_suit == Suit::HEARTS || m_suit == Suit::DIAMONDS)
        return Color::BRIGHT_RED;
    return Color::BRIGHT_WHITE;
}

std::string Card::toString() const {
    return getRankStr() + getSuitStr();
}

void Card::print() const {
    std::cout << getSuitColor() << Color::BOLD
              << "[" << getRankStr() << getSuitStr() << "]"
              << Color::RESET;
}

// ============================================================
//  Deck – implementacja
// ============================================================
Deck::Deck(int numDecks) : m_numDecks(numDecks) {
    reset();
    shuffle();
}

void Deck::reset() {
    m_cards.clear();
    for (int d = 0; d < m_numDecks; ++d) {
        for (int s = 0; s < 4; ++s) {
            for (int r = 2; r <= 14; ++r) {
                m_cards.emplace_back(static_cast<Suit>(s), static_cast<Rank>(r));
            }
        }
    }
}

void Deck::shuffle() {
    for (int i = (int)m_cards.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        std::swap(m_cards[i], m_cards[j]);
    }
}

Card Deck::deal() {
    if (m_cards.empty()) {
        reset();
        shuffle();
    }
    Card c = m_cards.back();
    m_cards.pop_back();
    return c;
}

// ============================================================
//  Hand – implementacja
// ============================================================
void Hand::print(bool hideFirst) const {
    for (int i = 0; i < (int)m_cards.size(); ++i) {
        if (i == 0 && hideFirst) {
            std::cout << Color::BLUE << Color::BOLD << "[??]" << Color::RESET;
        } else {
            m_cards[i].print();
        }
        if (i + 1 < (int)m_cards.size()) std::cout << " ";
    }
    std::cout << "\n";
}

std::string Hand::toString() const {
    std::string s;
    for (const auto& c : m_cards) s += c.toString() + " ";
    return s;
}

// ============================================================
//  BlackjackHand – implementacja
// ============================================================
int BlackjackHand::getValue() const {
    int value = 0;
    int aces  = 0;
    for (const auto& c : m_cards) {
        if (c.getRank() == Rank::ACE) {
            ++aces;
            value += 11;
        } else {
            value += c.getBlackjackValue();
        }
    }
    while (value > 21 && aces > 0) {
        value -= 10;
        --aces;
    }
    return value;
}

bool BlackjackHand::isSoft() const {
    int value = 0;
    int aces  = 0;
    for (const auto& c : m_cards) {
        if (c.getRank() == Rank::ACE) { ++aces; value += 11; }
        else                           value += c.getBlackjackValue();
    }
    // Jeśli as liczy się jako 11 i nie przekraczamy 21 – ręka miękka
    while (value > 21 && aces > 0) { value -= 10; --aces; }
    return (aces > 0 && value <= 21);
}

void BlackjackHand::print(bool hideFirst) const {
    for (int i = 0; i < (int)m_cards.size(); ++i) {
        if (i == 0 && hideFirst) {
            std::cout << Color::BLUE << Color::BOLD << "[  ?  ]" << Color::RESET;
        } else {
            m_cards[i].print();
        }
        if (i + 1 < (int)m_cards.size()) std::cout << "  ";
    }
    if (!hideFirst)
        std::cout << Color::DIM << "  (" << getValue() << ")" << Color::RESET;
    std::cout << "\n";
}
