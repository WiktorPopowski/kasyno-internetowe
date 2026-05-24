#ifndef ROULETTE_H
#define ROULETTE_H

#include "game.h"
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QVector>

struct RouletteBet {
    enum Type { STRAIGHT, RED, BLACK, ODD, EVEN, LOW, HIGH, DOZEN1, DOZEN2, DOZEN3 };
    Type type;
    int number; // dla STRAIGHT
    double amount;
    QString description;

    RouletteBet(Type t, int n, double a, const QString &d)
        : type(t), number(n), amount(a), description(d) {}
};

class RouletteGame : public Game
{
    Q_OBJECT

public:
    RouletteGame(Player *player, QWidget *parent = nullptr);
    QString getGameName() const override { return "Ruletka"; }
    void resetGame() override;

private slots:
    void spin();
    void updateAnimation();
    void finishSpin();
    void addBet();
    void clearBets();

private:
    static const int RED_NUMBERS[];

    QLabel *balanceLabel;
    QLabel *resultLabel;
    QLabel *wheelLabel;
    QLabel *currentBetsLabel;
    QLabel *totalBetLabel;
    QPushButton *spinButton;
    QPushButton *addBetButton;
    QPushButton *clearBetsButton;
    QSpinBox *betAmountSpin;
    QComboBox *betTypeCombo;
    QSpinBox *straightNumberSpin;

    QTimer *spinTimer;
    int animFrame;
    int finalNumber;

    QVector<RouletteBet> bets;
    double totalBetAmount;

    void setupUI();
    void updateBalance();
    void updateBetsList();
    bool isRed(int n) const;
    double calculateWinnings(int number);
    QString numberColor(int n) const;
};

#endif // ROULETTE_H
