#ifndef GAME_H
#define GAME_H

#include "common.h"
#include "player.h"
#include "card.h"

// ============================================================
//  Game – abstrakcyjna klasa bazowa dla wszystkich gier
// ============================================================
class Game {
protected:
    Player&     m_player;
    std::string m_name;

    // Pomocnicze: wyświetl baner gry
    void printGameBanner() const;

public:
    explicit Game(Player& player, const std::string& name)
        : m_player(player), m_name(name) {}

    virtual ~Game() = default;

    // Metoda czysto wirtualna – każda gra implementuje własną logikę
    virtual void run() = 0;

    // Pomocnicze – pobierz zakład
    double askForBet(double minBet = 1.0) const;

    std::string getName() const { return m_name; }
};

// ============================================================
//  BlackjackGame – Blackjack (21)
// ============================================================
class BlackjackGame : public Game {
private:
    Deck           m_deck;
    BlackjackHand  m_playerHand;
    BlackjackHand  m_dealerHand;

    void  printTable(bool hideDealer) const;
    bool  playerTurn(double bet);
    void  dealerTurn();
    void  resolveRound(double bet);
    bool  offerInsurance(double bet);

public:
    explicit BlackjackGame(Player& player);
    void run() override;
};

// ============================================================
//  RouletteGame – Ruletka europejska
// ============================================================
struct RouletteBet {
    enum class Type {
        STRAIGHT,       // pojedyncza liczba
        RED, BLACK,     // kolor
        EVEN, ODD,      // parzyste / nieparzyste
        LOW, HIGH,      // 1-18 / 19-36
        DOZEN_1, DOZEN_2, DOZEN_3,   // tuziny
        COLUMN_1, COLUMN_2, COLUMN_3 // kolumny
    };
    Type   type;
    int    number; // używane tylko przy STRAIGHT
    double amount;
};

class RouletteGame : public Game {
private:
    static const int RED_NUMBERS[];
    int  spin() const;
    bool isRed(int n) const;
    void printWheel(int result) const;
    void printBettingMenu() const;
    double resolveAllBets(const std::vector<RouletteBet>& bets, int result) const;
    double getPayoutMultiplier(const RouletteBet& bet, int result) const;

public:
    explicit RouletteGame(Player& player);
    void run() override;
};

// ============================================================
//  SlotMachine – Jednoreki bandyta (5 bębnów, 3 wiersze)
// ============================================================
class SlotMachine : public Game {
private:
    static const int REELS = 5;
    static const int ROWS  = 3;

    // Symbole i ich wagi (im wyższy index, tym rzadszy)
    struct Symbol {
        std::string display;
        std::string name;
        int         weight;
        int         payoutLine3; // za 3 z rzędu
        int         payoutLine4; // za 4 z rzędu
        int         payoutLine5; // za 5 z rzędu
    };
    static const std::vector<Symbol> SYMBOLS;

    int  m_grid[ROWS][REELS];
    int  m_balance_display;

    int  spinSymbol() const;
    void spinAllReels();
    double evaluateWin(double bet) const;
    void  printGrid() const;
    void  printPaytable() const;
    void  animateSpin() const;

public:
    explicit SlotMachine(Player& player);
    void run() override;
};

// ============================================================
//  VideoPoker – Poker wideo (Jacks or Better)
// ============================================================
class VideoPoker : public Game {
private:
    Deck m_deck;
    std::vector<Card> m_hand;

    enum class HandRank {
        HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_OF_A_KIND,
        STRAIGHT, FLUSH, FULL_HOUSE, FOUR_OF_A_KIND,
        STRAIGHT_FLUSH, ROYAL_FLUSH
    };

    HandRank evaluateHand() const;
    std::string handRankName(HandRank r) const;
    int         handRankPayout(HandRank r) const; // mnożnik x bet

    void printHand(const std::vector<bool>& held) const;
    void printPaytable() const;
    bool isStraight() const;
    bool isFlush()    const;
    bool isRoyalFlush() const;

public:
    explicit VideoPoker(Player& player);
    void run() override;
};

// ============================================================
//  DiceGame – Kości (uproszczone Craps)
// ============================================================
class DiceGame : public Game {
private:
    int rollDie() const;
    int rollTwo() const;
    void printDice(int d1, int d2) const;
    void printRules() const;

public:
    explicit DiceGame(Player& player);
    void run() override;
};

#endif // GAME_H
