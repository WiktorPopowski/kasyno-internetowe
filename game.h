#ifndef GAME_H
#define GAME_H

#include <QString>
#include <QWidget>
#include "player.h"

// Abstrakcyjna klasa bazowa dla wszystkich gier
class Game : public QWidget
{
    Q_OBJECT

public:
    Game(Player *player, QWidget *parent = nullptr)
        : QWidget(parent), player(player), currentBet(0.0) {}

    virtual ~Game() {}

    virtual QString getGameName() const = 0;
    virtual void resetGame() = 0;

signals:
    void balanceChanged();
    void gameMessage(const QString &msg);

protected:
    Player *player;
    double currentBet;

    bool placeBet(double amount) {
        if (player->placeBet(amount)) {
            currentBet = amount;
            emit balanceChanged();
            return true;
        }
        return false;
    }

    void awardWinnings(double multiplier) {
        double winAmount = currentBet * multiplier;
        player->addWinnings(winAmount);
        currentBet = 0.0;
        emit balanceChanged();
    }
};

#endif // GAME_H
