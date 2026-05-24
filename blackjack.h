#ifndef BLACKJACK_H
#define BLACKJACK_H

#include "game.h"
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVector>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>

struct Card {
    int value;      // 1-13 (1=As, 11=J, 12=Q, 13=K)
    int suit;       // 0=♠ 1=♥ 2=♦ 3=♣

    Card(int v, int s) : value(v), suit(s) {}
    QString toString() const;
    int blackjackValue() const;
    QString suitSymbol() const;
};

class Deck {
public:
    Deck();
    void shuffle();
    Card deal();
    bool isEmpty() const;

private:
    QVector<Card> cards;
    int top;
};

class BlackjackGame : public Game
{
    Q_OBJECT

public:
    BlackjackGame(Player *player, QWidget *parent = nullptr);
    QString getGameName() const override { return "Blackjack"; }
    void resetGame() override;

private slots:
    void dealCards();
    void playerHit();
    void playerStand();
    void playerDoubleDown();

private:
    enum class GameState { WAITING, PLAYING, FINISHED };
    GameState state;

    Deck deck;
    QVector<Card> playerHand;
    QVector<Card> dealerHand;

    QLabel *balanceLabel;
    QLabel *resultLabel;
    QLabel *playerScoreLabel;
    QLabel *dealerScoreLabel;
    QHBoxLayout *playerCardsLayout;
    QHBoxLayout *dealerCardsLayout;
    QFrame *playerCardsFrame;
    QFrame *dealerCardsFrame;
    QPushButton *dealButton;
    QPushButton *hitButton;
    QPushButton *standButton;
    QPushButton *doubleButton;
    QSpinBox *betSpinBox;

    void setupUI();
    void updateCardsDisplay(QHBoxLayout *layout, const QVector<Card> &hand, bool hideSecond = false);
    int calculateScore(const QVector<Card> &hand) const;
    void checkResult();
    void setPlayingMode(bool playing);
    void updateBalance();
    QFrame* createCardWidget(const Card &card, bool hidden = false);
};

#endif // BLACKJACK_H
