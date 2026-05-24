#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <QVector>
#include <QDateTime>

struct Transaction {
    QString type;
    double amount;
    double balanceAfter;
    QDateTime timestamp;

    Transaction(const QString &t, double a, double b)
        : type(t), amount(a), balanceAfter(b), timestamp(QDateTime::currentDateTime()) {}
};

class Player
{
public:
    Player(const QString &name = "Gracz", double initialBalance = 1000.0);

    QString getName() const;
    double getBalance() const;
    bool canBet(double amount) const;
    void addWinnings(double amount);
    bool placeBet(double amount);
    void deposit(double amount);

    const QVector<Transaction>& getHistory() const;
    void addTransaction(const QString &type, double amount);

private:
    QString name;
    double balance;
    QVector<Transaction> history;
};

#endif // PLAYER_H
