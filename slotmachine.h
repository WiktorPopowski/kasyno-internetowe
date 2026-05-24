#ifndef SLOTMACHINE_H
#define SLOTMACHINE_H

#include "game.h"
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QVector>
#include <QGridLayout>
#include <QFrame>

class SlotMachine : public Game
{
    Q_OBJECT

public:
    SlotMachine(Player *player, QWidget *parent = nullptr);
    QString getGameName() const override { return "Slot Machine"; }
    void resetGame() override;

private slots:
    void spin();
    void updateReels();
    void finishSpin();

private:
    static const int REELS = 3;
    static const int SYMBOLS_COUNT = 7;

    QLabel *reelLabels[REELS];
    QLabel *resultLabel;
    QLabel *balanceLabel;
    QLabel *betLabel;
    QPushButton *spinButton;
    QSpinBox *betSpinBox;
    QTimer *spinTimer;
    QTimer *stopTimer;

    QVector<QString> symbols;
    int reelValues[REELS];
    int spinCount;
    int stopStep;

    void setupUI();
    void updateBalance();
    double calculateWin(int r0, int r1, int r2);
    QString getSymbolEmoji(int idx);
    void animateReel(int reel);
    void applyReelStyle(QLabel *label, const QString &symbol);
};

#endif // SLOTMACHINE_H
