#ifndef CASINO_H
#define CASINO_H

#include "common.h"
#include "player.h"
#include "game.h"

// ============================================================
//  Casino – klasa zarządzająca całą aplikacją
//  (wzorzec Facade + zarządzanie grami przez wskaźniki na Game)
// ============================================================
class Casino {
private:
    Player*              m_player;
    std::vector<Game*>   m_games;   // polimorficzna lista gier
    bool                 m_running;

    // Ekrany
    void printWelcomeBanner() const;
    void printMainMenu()      const;
    void printCasinoLobby()   const;
    void printCashierMenu();
    void printAccountInfo()   const;

    // Akcje
    bool loginOrRegister();
    void runGameSelection();
    void cashierMenu();
    void showLeaderboard() const;

    // Pomocnicze
    void registerGames();
    void cleanup();

public:
    Casino();
    ~Casino();

    void start();
};

#endif // CASINO_H
