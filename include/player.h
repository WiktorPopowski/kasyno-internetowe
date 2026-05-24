#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

// ============================================================
//  TransactionRecord  – zapis pojedynczej transakcji
// ============================================================
struct TransactionRecord {
    std::string game;
    double      bet;
    double      result;  // > 0 wygrana, < 0 przegrana
    std::string detail;

    TransactionRecord(const std::string& g, double b, double r, const std::string& d)
        : game(g), bet(b), result(r), detail(d) {}
};

// ============================================================
//  Player – klasa gracza (konto kasyna)
// ============================================================
class Player {
private:
    std::string              m_name;
    double                   m_balance;
    double                   m_totalWon;
    double                   m_totalLost;
    int                      m_gamesPlayed;
    std::vector<TransactionRecord> m_history;
    static const int         MAX_HISTORY = 50;

public:
    explicit Player(const std::string& name, double startBalance = 1000.0);

    // Gettery
    std::string getName()       const { return m_name; }
    double      getBalance()    const { return m_balance; }
    double      getTotalWon()   const { return m_totalWon; }
    double      getTotalLost()  const { return m_totalLost; }
    int         getGamesPlayed()const { return m_gamesPlayed; }
    double      getNetProfit()  const { return m_totalWon - m_totalLost; }

    // Operacje na saldzie
    bool  placeBet(double amount);
    void  addWinnings(double amount);
    void  recordResult(const std::string& game, double bet,
                       double resultDelta, const std::string& detail);

    // Depozyt / wypłata
    void  deposit(double amount);
    bool  withdraw(double amount);

    // Wyświetlanie
    void  printHeader()  const;
    void  printStats()   const;
    void  printHistory() const;

    // Zapis / odczyt
    bool  saveToFile()   const;
    bool  loadFromFile(const std::string& name);
};

#endif // PLAYER_H
