#include "slotmachine.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QFont>

SlotMachine::SlotMachine(Player *player, QWidget *parent)
    : Game(player, parent), spinCount(0), stopStep(0)
{
    symbols = {"🍒", "🍋", "🍊", "🍇", "⭐", "💎", "7️⃣"};
    for (int i = 0; i < REELS; i++) reelValues[i] = 0;

    spinTimer = new QTimer(this);
    stopTimer = new QTimer(this);
    connect(spinTimer, &QTimer::timeout, this, &SlotMachine::updateReels);
    connect(stopTimer, &QTimer::timeout, this, &SlotMachine::finishSpin);

    setupUI();
    updateBalance();
}

void SlotMachine::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(20);

    // Title
    QLabel *title = new QLabel("🎰 SLOT MACHINE");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #FFD700; "
                         "font-family: 'Segoe UI'; letter-spacing: 3px;");
    mainLayout->addWidget(title);

    // Balance display
    balanceLabel = new QLabel();
    balanceLabel->setAlignment(Qt::AlignCenter);
    balanceLabel->setStyleSheet("font-size: 18px; color: #00FF88; font-family: 'Consolas';");
    mainLayout->addWidget(balanceLabel);

    // Reels container
    QFrame *reelFrame = new QFrame();
    reelFrame->setStyleSheet("QFrame { background: #1a0a2e; border: 3px solid #FFD700; "
                              "border-radius: 15px; padding: 10px; }");
    QHBoxLayout *reelLayout = new QHBoxLayout(reelFrame);
    reelLayout->setSpacing(15);

    for (int i = 0; i < REELS; i++) {
        QFrame *singleReel = new QFrame();
        singleReel->setFixedSize(130, 130);
        singleReel->setStyleSheet("QFrame { background: #0d0d1a; border: 2px solid #555; "
                                   "border-radius: 10px; }");
        QVBoxLayout *sl = new QVBoxLayout(singleReel);
        sl->setAlignment(Qt::AlignCenter);

        reelLabels[i] = new QLabel("🍒");
        reelLabels[i]->setAlignment(Qt::AlignCenter);
        reelLabels[i]->setStyleSheet("font-size: 52px;");
        sl->addWidget(reelLabels[i]);

        reelLayout->addWidget(singleReel);
    }
    mainLayout->addWidget(reelFrame, 0, Qt::AlignCenter);

    // Result label
    resultLabel = new QLabel("Naciśnij SPIN aby zagrać!");
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet("font-size: 16px; color: #AAAAAA; font-family: 'Segoe UI';");
    mainLayout->addWidget(resultLabel);

    // Bet controls
    QHBoxLayout *betLayout = new QHBoxLayout();
    betLayout->setAlignment(Qt::AlignCenter);

    QLabel *betLbl = new QLabel("Zakład (zł):");
    betLbl->setStyleSheet("color: #CCCCCC; font-size: 15px;");
    betLayout->addWidget(betLbl);

    betSpinBox = new QSpinBox();
    betSpinBox->setRange(10, 1000);
    betSpinBox->setValue(50);
    betSpinBox->setSingleStep(10);
    betSpinBox->setStyleSheet("QSpinBox { background: #1a0a2e; color: #FFD700; "
                               "border: 2px solid #FFD700; border-radius: 8px; "
                               "padding: 5px; font-size: 15px; min-width: 90px; }");
    betLayout->addWidget(betSpinBox);
    mainLayout->addLayout(betLayout);

    // Spin button
    spinButton = new QPushButton("🎰  SPIN!");
    spinButton->setFixedSize(200, 55);
    spinButton->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #FF6B35, stop:1 #FFD700); color: #0d0d1a; font-size: 18px; "
        "font-weight: bold; border-radius: 27px; border: none; }"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #FFD700, stop:1 #FF6B35); }"
        "QPushButton:disabled { background: #555; color: #888; }");
    connect(spinButton, &QPushButton::clicked, this, &SlotMachine::spin);
    mainLayout->addWidget(spinButton, 0, Qt::AlignCenter);

    // Paytable
    QLabel *payLabel = new QLabel(
        "TABELA WYPŁAT: 💎💎💎 = 20x  |  7️⃣7️⃣7️⃣ = 15x  |  ⭐⭐⭐ = 10x  |  "
        "🍇🍇🍇 = 8x  |  Inne trójki = 5x  |  Para = 1.5x");
    payLabel->setAlignment(Qt::AlignCenter);
    payLabel->setStyleSheet("font-size: 11px; color: #777; font-family: 'Consolas';");
    payLabel->setWordWrap(true);
    mainLayout->addWidget(payLabel);
}

void SlotMachine::updateBalance()
{
    balanceLabel->setText(QString("Saldo: %1 zł").arg(player->getBalance(), 0, 'f', 2));
}

void SlotMachine::spin()
{
    double bet = betSpinBox->value();
    if (!player->canBet(bet)) {
        QMessageBox::warning(this, "Brak środków",
                             "Niewystarczające saldo! Wpłać więcej środków.");
        return;
    }
    player->placeBet(bet);
    currentBet = bet;
    emit balanceChanged();
    updateBalance();

    spinButton->setEnabled(false);
    resultLabel->setText("Kręcę...");
    resultLabel->setStyleSheet("font-size: 16px; color: #FFD700;");

    spinCount = 0;
    spinTimer->start(80);
    stopTimer->start(1500);
}

void SlotMachine::updateReels()
{
    for (int i = 0; i < REELS; i++) {
        int idx = QRandomGenerator::global()->bounded(SYMBOLS_COUNT);
        reelLabels[i]->setText(symbols[idx]);
    }
    spinCount++;
}

void SlotMachine::finishSpin()
{
    stopTimer->stop();
    spinTimer->stop();

    for (int i = 0; i < REELS; i++) {
        reelValues[i] = QRandomGenerator::global()->bounded(SYMBOLS_COUNT);
        reelLabels[i]->setText(symbols[reelValues[i]]);
    }

    double multiplier = calculateWin(reelValues[0], reelValues[1], reelValues[2]);

    if (multiplier > 0) {
        double winAmount = currentBet * multiplier;
        player->addWinnings(winAmount);
        emit balanceChanged();
        resultLabel->setText(QString("🎉 WYGRAŁEŚ %1 zł! (x%2)").arg(winAmount, 0, 'f', 2).arg(multiplier));
        resultLabel->setStyleSheet("font-size: 17px; color: #00FF88; font-weight: bold;");
    } else {
        resultLabel->setText("Nie tym razem... Spróbuj jeszcze raz!");
        resultLabel->setStyleSheet("font-size: 16px; color: #FF4444;");
    }
    currentBet = 0;
    updateBalance();
    spinButton->setEnabled(true);
}

double SlotMachine::calculateWin(int r0, int r1, int r2)
{
    if (r0 == r1 && r1 == r2) {
        if (r0 == 5) return 20.0; // 💎
        if (r0 == 6) return 15.0; // 7
        if (r0 == 4) return 10.0; // ⭐
        if (r0 == 3) return 8.0;  // 🍇
        return 5.0;
    }
    if (r0 == r1 || r1 == r2 || r0 == r2) return 1.5;
    return 0.0;
}

void SlotMachine::resetGame()
{
    resultLabel->setText("Naciśnij SPIN aby zagrać!");
    resultLabel->setStyleSheet("font-size: 16px; color: #AAAAAA;");
    updateBalance();
    spinButton->setEnabled(true);
}
