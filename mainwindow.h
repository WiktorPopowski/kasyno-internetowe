#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTableWidget>
#include "player.h"
#include "slotmachine.h"
#include "blackjack.h"
#include "roulette.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showLobby();
    void showSlotMachine();
    void showBlackjack();
    void showRoulette();
    void showHistory();
    void showDeposit();
    void processDeposit();
    void updateBalanceDisplay();

private:
    QStackedWidget *stackedWidget;
    Player *player;

    // Pages
    QWidget *loginPage;
    QWidget *lobbyPage;
    QWidget *slotPage;
    QWidget *blackjackPage;
    QWidget *roulettePage;
    QWidget *historyPage;
    QWidget *depositPage;

    // Games
    SlotMachine *slotGame;
    BlackjackGame *blackjackGame;
    RouletteGame *rouletteGame;

    // UI elements
    QLabel *balanceLabel;
    QLabel *headerBalanceLabel;
    QLineEdit *depositAmountEdit;
    QLineEdit *playerNameEdit;
    QTableWidget *historyTable;

    void setupLoginPage();
    void setupLobbyPage();
    void setupSlotPage();
    void setupBlackjackPage();
    void setupRoulettePage();
    void setupHistoryPage();
    void setupDepositPage();
    QWidget* createHeader(const QString &title, bool showBack = true);
    void applyGlobalStyle();

private slots:
    void handleLogin();
};

#endif // MAINWINDOW_H
