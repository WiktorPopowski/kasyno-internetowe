#include "roulette.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include <QScrollArea>

const int RouletteGame::RED_NUMBERS[] = {
    1,3,5,7,9,12,14,16,18,19,21,23,25,27,30,32,34,36
};

RouletteGame::RouletteGame(Player *player, QWidget *parent)
    : Game(player, parent), animFrame(0), finalNumber(0), totalBetAmount(0)
{
    spinTimer = new QTimer(this);
    connect(spinTimer, &QTimer::timeout, this, &RouletteGame::updateAnimation);
    setupUI();
    updateBalance();
}

void RouletteGame::setupUI()
{
    QVBoxLayout *main = new QVBoxLayout(this);
    main->setSpacing(12);

    QLabel *title = new QLabel("🎡 RULETKA EUROPEJSKA");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: #FF6B35; "
                         "font-family: 'Segoe UI'; letter-spacing: 3px;");
    main->addWidget(title);

    balanceLabel = new QLabel();
    balanceLabel->setAlignment(Qt::AlignCenter);
    balanceLabel->setStyleSheet("font-size: 16px; color: #FFD700;");
    main->addWidget(balanceLabel);

    // Wheel display
    QFrame *wheelFrame = new QFrame();
    wheelFrame->setFixedHeight(110);
    wheelFrame->setStyleSheet("background: #1a2a1a; border: 3px solid #2d8a2d; border-radius: 55px;");
    QVBoxLayout *wfl = new QVBoxLayout(wheelFrame);
    wfl->setAlignment(Qt::AlignCenter);

    wheelLabel = new QLabel("🎡  0");
    wheelLabel->setAlignment(Qt::AlignCenter);
    wheelLabel->setStyleSheet("font-size: 40px; color: #00FF44;");
    wfl->addWidget(wheelLabel);
    main->addWidget(wheelFrame, 0, Qt::AlignCenter);
    wheelFrame->setFixedWidth(250);

    resultLabel = new QLabel("Postaw zakłady i kręć kołem!");
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet("font-size: 15px; color: #AAAAAA;");
    main->addWidget(resultLabel);

    // Betting panel
    QFrame *betPanel = new QFrame();
    betPanel->setStyleSheet("QFrame { background: #1a1a2e; border: 1px solid #333; border-radius: 10px; padding: 5px; }");
    QGridLayout *betGrid = new QGridLayout(betPanel);
    betGrid->setSpacing(8);

    QLabel *betTypeLbl = new QLabel("Typ zakładu:");
    betTypeLbl->setStyleSheet("color: #CCC; font-size: 14px;");
    betGrid->addWidget(betTypeLbl, 0, 0);

    betTypeCombo = new QComboBox();
    betTypeCombo->addItems({
        "Konkretna liczba (0-36)",
        "Czerwone",
        "Czarne",
        "Nieparzyste",
        "Parzyste",
        "Niskie (1-18)",
        "Wysokie (19-36)",
        "Pierwsza dwunastka (1-12)",
        "Druga dwunastka (13-24)",
        "Trzecia dwunastka (25-36)"
    });
    betTypeCombo->setStyleSheet("QComboBox { background: #0d0d1a; color: #CCC; border: 1px solid #555; "
                                 "border-radius: 5px; padding: 4px; font-size: 13px; } "
                                 "QComboBox::drop-down { border: none; } "
                                 "QComboBox QAbstractItemView { background: #1a1a2e; color: #CCC; }");
    betGrid->addWidget(betTypeCombo, 0, 1, 1, 2);

    QLabel *numLbl = new QLabel("Liczba:");
    numLbl->setStyleSheet("color: #CCC; font-size: 14px;");
    betGrid->addWidget(numLbl, 1, 0);

    straightNumberSpin = new QSpinBox();
    straightNumberSpin->setRange(0, 36);
    straightNumberSpin->setStyleSheet("QSpinBox { background: #0d0d1a; color: #FFD700; "
                                       "border: 1px solid #555; border-radius: 5px; padding: 4px; }");
    betGrid->addWidget(straightNumberSpin, 1, 1);

    connect(betTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int idx) {
        straightNumberSpin->setEnabled(idx == 0);
    });

    QLabel *amtLbl = new QLabel("Kwota (zł):");
    amtLbl->setStyleSheet("color: #CCC; font-size: 14px;");
    betGrid->addWidget(amtLbl, 2, 0);

    betAmountSpin = new QSpinBox();
    betAmountSpin->setRange(5, 5000);
    betAmountSpin->setValue(50);
    betAmountSpin->setSingleStep(25);
    betAmountSpin->setStyleSheet("QSpinBox { background: #0d0d1a; color: #00FF88; "
                                  "border: 1px solid #00FF88; border-radius: 5px; padding: 4px; }");
    betGrid->addWidget(betAmountSpin, 2, 1);

    addBetButton = new QPushButton("➕ Dodaj zakład");
    addBetButton->setStyleSheet("QPushButton { background: #1a6b3a; color: white; border-radius: 6px; "
                                 "padding: 6px 12px; font-size: 13px; font-weight: bold; } "
                                 "QPushButton:hover { background: #2a9b5a; }");
    betGrid->addWidget(addBetButton, 2, 2);

    main->addWidget(betPanel);
    connect(addBetButton, &QPushButton::clicked, this, &RouletteGame::addBet);

    // Current bets display
    currentBetsLabel = new QLabel("Brak zakładów");
    currentBetsLabel->setAlignment(Qt::AlignLeft);
    currentBetsLabel->setWordWrap(true);
    currentBetsLabel->setStyleSheet("color: #AAA; font-size: 12px; font-family: 'Consolas';");
    main->addWidget(currentBetsLabel);

    totalBetLabel = new QLabel("Łączny zakład: 0 zł");
    totalBetLabel->setStyleSheet("color: #FFD700; font-size: 14px; font-weight: bold;");
    main->addWidget(totalBetLabel);

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setAlignment(Qt::AlignCenter);
    btnRow->setSpacing(15);

    spinButton = new QPushButton("🎡  KRĘĆ KOŁEM");
    spinButton->setFixedSize(200, 50);
    spinButton->setStyleSheet("QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                               "stop:0 #CC2200, stop:1 #FF6B35); color: white; font-size: 16px; "
                               "font-weight: bold; border-radius: 25px; border: none; } "
                               "QPushButton:disabled { background: #444; color: #777; }");
    connect(spinButton, &QPushButton::clicked, this, &RouletteGame::spin);
    btnRow->addWidget(spinButton);

    clearBetsButton = new QPushButton("🗑 Wyczyść");
    clearBetsButton->setFixedSize(120, 50);
    clearBetsButton->setStyleSheet("QPushButton { background: #6a1a1a; color: white; font-size: 13px; "
                                    "font-weight: bold; border-radius: 25px; border: none; } "
                                    "QPushButton:hover { background: #8a2a2a; }");
    connect(clearBetsButton, &QPushButton::clicked, this, &RouletteGame::clearBets);
    btnRow->addWidget(clearBetsButton);

    main->addLayout(btnRow);

    // Paytable
    QLabel *pay = new QLabel("WYPŁATY: Liczba = 35:1  |  Kolor/Parzyste/Zakres = 1:1  |  Dwunastka = 2:1");
    pay->setAlignment(Qt::AlignCenter);
    pay->setStyleSheet("font-size: 11px; color: #555; font-family: 'Consolas';");
    main->addWidget(pay);
}

void RouletteGame::addBet()
{
    int typeIdx = betTypeCombo->currentIndex();
    double amount = betAmountSpin->value();

    if (!player->canBet(amount)) {
        QMessageBox::warning(this, "Brak środków", "Niewystarczające saldo!");
        return;
    }

    RouletteBet::Type type;
    int number = 0;
    QString desc;

    switch(typeIdx) {
    case 0: type = RouletteBet::STRAIGHT; number = straightNumberSpin->value();
            desc = QString("Liczba %1").arg(number); break;
    case 1: type = RouletteBet::RED;   desc = "Czerwone"; break;
    case 2: type = RouletteBet::BLACK; desc = "Czarne"; break;
    case 3: type = RouletteBet::ODD;   desc = "Nieparzyste"; break;
    case 4: type = RouletteBet::EVEN;  desc = "Parzyste"; break;
    case 5: type = RouletteBet::LOW;   desc = "Niskie (1-18)"; break;
    case 6: type = RouletteBet::HIGH;  desc = "Wysokie (19-36)"; break;
    case 7: type = RouletteBet::DOZEN1; desc = "1. dwunastka"; break;
    case 8: type = RouletteBet::DOZEN2; desc = "2. dwunastka"; break;
    case 9: type = RouletteBet::DOZEN3; desc = "3. dwunastka"; break;
    default: return;
    }

    player->placeBet(amount);
    totalBetAmount += amount;
    emit balanceChanged();

    bets.append(RouletteBet(type, number, amount, desc));
    updateBetsList();
    updateBalance();
}

void RouletteGame::clearBets()
{
    // Return money for current bets
    player->addWinnings(totalBetAmount);
    totalBetAmount = 0;
    bets.clear();
    updateBetsList();
    updateBalance();
    emit balanceChanged();
}

void RouletteGame::updateBetsList()
{
    if (bets.isEmpty()) {
        currentBetsLabel->setText("Brak zakładów");
        totalBetLabel->setText("Łączny zakład: 0 zł");
        return;
    }
    QString txt;
    for (const auto &b : bets) {
        txt += QString("• %1: %2 zł\n").arg(b.description).arg(b.amount, 0, 'f', 2);
    }
    currentBetsLabel->setText(txt.trimmed());
    totalBetLabel->setText(QString("Łączny zakład: %1 zł").arg(totalBetAmount, 0, 'f', 2));
}

bool RouletteGame::isRed(int n) const
{
    for (int r : RED_NUMBERS) if (r == n) return true;
    return false;
}

QString RouletteGame::numberColor(int n) const
{
    if (n == 0) return "🟢";
    return isRed(n) ? "🔴" : "⚫";
}

void RouletteGame::spin()
{
    if (bets.isEmpty()) {
        QMessageBox::information(this, "Brak zakładów", "Postaw zakład przed kręceniem kołem!");
        return;
    }
    spinButton->setEnabled(false);
    addBetButton->setEnabled(false);
    clearBetsButton->setEnabled(false);

    finalNumber = QRandomGenerator::global()->bounded(37); // 0-36
    animFrame = 0;
    spinTimer->start(60);

    QTimer::singleShot(2500, this, &RouletteGame::finishSpin);
}

void RouletteGame::updateAnimation()
{
    int displayNum = QRandomGenerator::global()->bounded(37);
    wheelLabel->setText(QString("🎡  %1").arg(displayNum));
    animFrame++;
}

void RouletteGame::finishSpin()
{
    spinTimer->stop();
    wheelLabel->setText(QString("%1  %2").arg(numberColor(finalNumber)).arg(finalNumber));

    double totalWin = calculateWinnings(finalNumber);
    QString colorStr = (finalNumber == 0) ? "ZIELONY (0)" : (isRed(finalNumber) ? "CZERWONY" : "CZARNY");

    if (totalWin > 0) {
        player->addWinnings(totalWin);
        resultLabel->setText(QString("🎉 Wypadło %1 (%2)! Wygrałeś %3 zł!")
                             .arg(finalNumber).arg(colorStr).arg(totalWin, 0, 'f', 2));
        resultLabel->setStyleSheet("font-size: 15px; color: #00FF88; font-weight: bold;");
    } else {
        resultLabel->setText(QString("💔 Wypadło %1 (%2). Nie tym razem!")
                             .arg(finalNumber).arg(colorStr));
        resultLabel->setStyleSheet("font-size: 15px; color: #FF4444;");
    }

    emit balanceChanged();
    totalBetAmount = 0;
    bets.clear();
    updateBetsList();
    updateBalance();

    spinButton->setEnabled(true);
    addBetButton->setEnabled(true);
    clearBetsButton->setEnabled(true);
}

double RouletteGame::calculateWinnings(int number)
{
    double total = 0;
    bool red = isRed(number);
    bool zero = (number == 0);

    for (const RouletteBet &b : bets) {
        bool win = false;
        double mult = 0;

        switch(b.type) {
        case RouletteBet::STRAIGHT:
            win = (b.number == number);
            mult = 36; break; // 35:1 + zwrot
        case RouletteBet::RED:
            win = (!zero && red); mult = 2; break;
        case RouletteBet::BLACK:
            win = (!zero && !red); mult = 2; break;
        case RouletteBet::ODD:
            win = (!zero && number % 2 == 1); mult = 2; break;
        case RouletteBet::EVEN:
            win = (!zero && number % 2 == 0); mult = 2; break;
        case RouletteBet::LOW:
            win = (number >= 1 && number <= 18); mult = 2; break;
        case RouletteBet::HIGH:
            win = (number >= 19 && number <= 36); mult = 2; break;
        case RouletteBet::DOZEN1:
            win = (number >= 1 && number <= 12); mult = 3; break;
        case RouletteBet::DOZEN2:
            win = (number >= 13 && number <= 24); mult = 3; break;
        case RouletteBet::DOZEN3:
            win = (number >= 25 && number <= 36); mult = 3; break;
        }

        if (win) total += b.amount * mult;
    }
    return total;
}

void RouletteGame::updateBalance()
{
    balanceLabel->setText(QString("Saldo: %1 zł").arg(player->getBalance(), 0, 'f', 2));
}

void RouletteGame::resetGame()
{
    if (totalBetAmount > 0) {
        player->addWinnings(totalBetAmount);
        totalBetAmount = 0;
    }
    bets.clear();
    updateBetsList();
    resultLabel->setText("Postaw zakłady i kręć kołem!");
    resultLabel->setStyleSheet("font-size: 15px; color: #AAAAAA;");
    wheelLabel->setText("🎡  0");
    spinButton->setEnabled(true);
    addBetButton->setEnabled(true);
    clearBetsButton->setEnabled(true);
    updateBalance();
    emit balanceChanged();
}
