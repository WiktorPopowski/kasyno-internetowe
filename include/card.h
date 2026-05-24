#ifndef CARD_H
#define CARD_H

#include "common.h"

// ============================================================
//  Suit – kolor karty
// ============================================================
enum class Suit { HEARTS, DIAMONDS, CLUBS, SPADES };

// ============================================================
//  Rank – wartość karty
// ============================================================
enum class Rank {
    TWO=2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT,
    NINE, TEN, JACK, QUEEN, KING, ACE
};

// ============================================================
//  Card – pojedyncza karta
// ============================================================
class Card {
private:
    Suit m_suit;
    Rank m_rank;

public:
    Card(Suit s, Rank r) : m_suit(s), m_rank(r) {}

    Suit getSuit() const { return m_suit; }
    Rank getRank() const { return m_rank; }

    int  getBlackjackValue() const;
    std::string getRankStr() const;
    std::string getSuitStr() const;  // Unicode symbole
    std::string getSuitColor() const;
    std::string toString()   const;
    void         print()     const;
};

// ============================================================
//  Deck – talia kart (1 lub wiele talii)
// ============================================================
class Deck {
private:
    std::vector<Card> m_cards;
    int               m_numDecks;

public:
    explicit Deck(int numDecks = 1);

    void  reset();
    void  shuffle();
    Card  deal();
    int   remaining() const { return (int)m_cards.size(); }
    bool  needsReshuffle(int threshold = 15) const { return remaining() < threshold; }
};

// ============================================================
//  Hand – ręka gracza (zbiór kart)
// ============================================================
class Hand {
protected:
    std::vector<Card> m_cards;

public:
    Hand() = default;
    virtual ~Hand() = default;

    void  addCard(const Card& c) { m_cards.push_back(c); }
    void  clear()                { m_cards.clear(); }
    int   size()           const { return (int)m_cards.size(); }
    const std::vector<Card>& getCards() const { return m_cards; }

    virtual void print(bool hideFirst = false) const;
    virtual std::string toString() const;
};

// ============================================================
//  BlackjackHand – rozszerzona ręka do Blackjacka
// ============================================================
class BlackjackHand : public Hand {
public:
    int  getValue()      const;
    bool isBust()        const { return getValue() > 21; }
    bool isBlackjack()   const { return size() == 2 && getValue() == 21; }
    bool isSoft()        const;  // czy as liczy się jako 11

    void print(bool hideFirst = false) const override;
};

#endif // CARD_H
