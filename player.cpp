#include "player.h"

Player::Player(const QString &name, double initialBalance)
    : name(name), balance(initialBalance)
{
    addTransaction("Bonus powitalny", initialBalance);
}

QString Player::getName() const { return name; }
double Player::getBalance() const { return balance; }

bool Player::canBet(double amount) const {
    return amount > 0 && amount <= balance;
}

void Player::addWinnings(double amount) {
    balance += amount;
    addTransaction("Wygrana", amount);
}

bool Player::placeBet(double amount) {
    if (!canBet(amount)) return false;
    balance -= amount;
    addTransaction("Zakład", -amount);
    return true;
}

void Player::deposit(double amount) {
    if (amount <= 0) return;
    balance += amount;
    addTransaction("Wpłata", amount);
}

const QVector<Transaction>& Player::getHistory() const {
    return history;
}

void Player::addTransaction(const QString &type, double amount) {
    history.append(Transaction(type, amount, balance));
}
