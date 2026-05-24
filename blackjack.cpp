#include "blackjack.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <algorithm>

// ===== Card =====
QString Card::suitSymbol() const {
    switch(suit) {
    case 0: return "♠";
    case 1: return "♥";
    case 2: return "♦";
    case 3: return "♣";
    }
    return "";
}

QString Card::toString() const {
    QString v;
    if (value == 1) v = "A";
    else if (value == 11) v = "J";
    else if (value == 12) v = "Q";
    else if (value == 13) v = "K";
    else v = QString::number(value);
    return v + suitSymbol();
}

int Card::blackjackValue() const {
    if (value >= 10) return 10;
    return value;
}

// ===== Deck =====
Deck::Deck() : top(0) {
    for (int s = 0; s < 4; s++)
        for (int v = 1; v <= 13; v++)
            cards.append(Card(v, s));
    shuffle();
}

void Deck::shuffle() {
    top = 0;
    for (int i = cards.size() - 1; i > 0; i--) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        std::swap(cards[i], cards[j]);
    }
}

Card Deck::deal() {
    if (top >= cards.size()) {
        shuffle();
    }
    return cards[top++];
}

bool Deck::isEmpty() const { return top >= cards.size(); }

// ===== BlackjackGame =====
BlackjackGame::BlackjackGame(Player *player, QWidget *parent)
    : Game(player, parent), state(GameState::WAITING)
{
    setupUI();
    updateBalance();
}

void BlackjackGame::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setSpacing(15);

    QLabel *title = new QLabel("🃏 BLACKJACK");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #00FF88; "
                         "font-family: 'Segoe UI'; letter-spacing: 3px;");
    mainLayout->addWidget(title);

    balanceLabel = new QLabel();
    balanceLabel->setAlignment(Qt::AlignCenter);
    balanceLabel->setStyleSheet("font-size: 16px; color: #FFD700;");
    mainLayout->addWidget(balanceLabel);

    // Dealer area
    QLabel *dealerLbl = new QLabel("Krupier:");
    dealerLbl->setStyleSheet("color: #CCCCCC; font-size: 15px; font-weight: bold;");
    mainLayout->addWidget(dealerLbl);

    dealerScoreLabel = new QLabel("Punkty: -");
    dealerScoreLabel->setStyleSheet("color: #AAAAAA; font-size: 14px;");
    mainLayout->addWidget(dealerScoreLabel);

    dealerCardsFrame = new QFrame();
    dealerCardsFrame->setMinimumHeight(100);
    dealerCardsFrame->setStyleSheet("background: transparent;");
    dealerCardsLayout = new QHBoxLayout(dealerCardsFrame);
    dealerCardsLayout->setAlignment(Qt::AlignLeft);
    dealerCardsLayout->setSpacing(8);
    mainLayout->addWidget(dealerCardsFrame);

    // Divider
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #333;");
    mainLayout->addWidget(line);

    // Player area
    QLabel *playerLbl = new QLabel("Twoje karty:");
    playerLbl->setStyleSheet("color: #CCCCCC; font-size: 15px; font-weight: bold;");
    mainLayout->addWidget(playerLbl);

    playerScoreLabel = new QLabel("Punkty: -");
    playerScoreLabel->setStyleSheet("color: #AAAAAA; font-size: 14px;");
    mainLayout->addWidget(playerScoreLabel);

    playerCardsFrame = new QFrame();
    playerCardsFrame->setMinimumHeight(100);
    playerCardsFrame->setStyleSheet("background: transparent;");
    playerCardsLayout = new QHBoxLayout(playerCardsFrame);
    playerCardsLayout->setAlignment(Qt::AlignLeft);
    playerCardsLayout->setSpacing(8);
    mainLayout->addWidget(playerCardsFrame);

    // Result
    resultLabel = new QLabel("");
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    mainLayout->addWidget(resultLabel);

    // Bet + Deal
    QHBoxLayout *betLayout = new QHBoxLayout();
    betLayout->setAlignment(Qt::AlignCenter);

    QLabel *betLbl = new QLabel("Zakład:");
    betLbl->setStyleSheet("color: #CCC; font-size: 15px;");
    betLayout->addWidget(betLbl);

    betSpinBox = new QSpinBox();
    betSpinBox->setRange(10, 2000);
    betSpinBox->setValue(100);
    betSpinBox->setSingleStep(25);
    betSpinBox->setStyleSheet("QSpinBox { background: #1a1a2e; color: #00FF88; "
                               "border: 2px solid #00FF88; border-radius: 6px; "
                               "padding: 4px; font-size: 15px; min-width: 90px; }");
    betLayout->addWidget(betSpinBox);
    mainLayout->addLayout(betLayout);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setAlignment(Qt::AlignCenter);
    btnLayout->setSpacing(12);

    auto makeBtn = [](const QString &text, const QString &color) {
        QPushButton *btn = new QPushButton(text);
        btn->setFixedSize(130, 45);
        btn->setStyleSheet(QString(
            "QPushButton { background: %1; color: white; font-size: 14px; "
            "font-weight: bold; border-radius: 8px; border: none; }"
            "QPushButton:hover { opacity: 0.85; }"
            "QPushButton:disabled { background: #444; color: #888; }").arg(color));
        return btn;
    };

    dealButton   = makeBtn("🃏 Rozdaj",     "#1a6b3a");
    hitButton    = makeBtn("➕ Dobierz",    "#1a4a8a");
    standButton  = makeBtn("✋ Stój",       "#8a3a1a");
    doubleButton = makeBtn("⬆️ Double Down", "#6a1a8a");

    btnLayout->addWidget(dealButton);
    btnLayout->addWidget(hitButton);
    btnLayout->addWidget(standButton);
    btnLayout->addWidget(doubleButton);
    mainLayout->addLayout(btnLayout);

    connect(dealButton,   &QPushButton::clicked, this, &BlackjackGame::dealCards);
    connect(hitButton,    &QPushButton::clicked, this, &BlackjackGame::playerHit);
    connect(standButton,  &QPushButton::clicked, this, &BlackjackGame::playerStand);
    connect(doubleButton, &QPushButton::clicked, this, &BlackjackGame::playerDoubleDown);

    setPlayingMode(false);
}

QFrame* BlackjackGame::createCardWidget(const Card &card, bool hidden)
{
    QFrame *f = new QFrame();
    f->setFixedSize(65, 90);

    if (hidden) {
        f->setStyleSheet("QFrame { background: #2c4a8a; border: 2px solid #5566aa; "
                         "border-radius: 8px; }");
        QLabel *l = new QLabel("?", f);
        l->setAlignment(Qt::AlignCenter);
        l->setGeometry(0, 0, 65, 90);
        l->setStyleSheet("font-size: 30px; color: #8899cc;");
        return f;
    }

    bool red = (card.suit == 1 || card.suit == 2);
    QString bg = "white";
    QString textColor = red ? "#CC0000" : "#111111";

    f->setStyleSheet(QString("QFrame { background: %1; border: 2px solid #999; "
                             "border-radius: 8px; }").arg(bg));

    QString vStr;
    if (card.value == 1) vStr = "A";
    else if (card.value == 11) vStr = "J";
    else if (card.value == 12) vStr = "Q";
    else if (card.value == 13) vStr = "K";
    else vStr = QString::number(card.value);

    QLabel *topL = new QLabel(vStr + "\n" + card.suitSymbol(), f);
    topL->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    topL->setGeometry(4, 2, 30, 40);
    topL->setStyleSheet(QString("font-size: 12px; font-weight: bold; color: %1; "
                                "background: transparent;").arg(textColor));

    QLabel *midL = new QLabel(card.suitSymbol(), f);
    midL->setAlignment(Qt::AlignCenter);
    midL->setGeometry(0, 25, 65, 40);
    midL->setStyleSheet(QString("font-size: 24px; color: %1; background: transparent;").arg(textColor));

    return f;
}

void BlackjackGame::updateCardsDisplay(QHBoxLayout *layout, const QVector<Card> &hand, bool hideSecond)
{
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    for (int i = 0; i < hand.size(); i++) {
        bool hidden = (hideSecond && i == 1);
        layout->addWidget(createCardWidget(hand[i], hidden));
    }
    layout->addStretch();
}

int BlackjackGame::calculateScore(const QVector<Card> &hand) const
{
    int score = 0;
    int aces = 0;
    for (const Card &c : hand) {
        score += c.blackjackValue();
        if (c.value == 1) aces++;
    }
    while (aces > 0 && score + 10 <= 21) {
        score += 10;
        aces--;
    }
    return score;
}

void BlackjackGame::dealCards()
{
    double bet = betSpinBox->value();
    if (!player->canBet(bet)) {
        QMessageBox::warning(this, "Brak środków", "Niewystarczające saldo!");
        return;
    }
    player->placeBet(bet);
    currentBet = bet;
    emit balanceChanged();

    deck.shuffle();
    playerHand.clear();
    dealerHand.clear();
    resultLabel->setText("");

    playerHand.append(deck.deal());
    dealerHand.append(deck.deal());
    playerHand.append(deck.deal());
    dealerHand.append(deck.deal());

    updateCardsDisplay(playerCardsLayout, playerHand);
    updateCardsDisplay(dealerCardsLayout, dealerHand, true);

    int ps = calculateScore(playerHand);
    playerScoreLabel->setText(QString("Punkty: %1").arg(ps));
    dealerScoreLabel->setText(QString("Punkty: %1 + ?").arg(dealerHand[0].blackjackValue()));

    state = GameState::PLAYING;
    setPlayingMode(true);
    updateBalance();

    if (ps == 21) {
        resultLabel->setText("🎉 BLACKJACK!");
        playerStand();
    }
}

void BlackjackGame::playerHit()
{
    playerHand.append(deck.deal());
    updateCardsDisplay(playerCardsLayout, playerHand);
    int ps = calculateScore(playerHand);
    playerScoreLabel->setText(QString("Punkty: %1").arg(ps));

    if (ps > 21) {
        updateCardsDisplay(dealerCardsLayout, dealerHand);
        dealerScoreLabel->setText(QString("Punkty: %1").arg(calculateScore(dealerHand)));
        resultLabel->setText("💥 Przekroczyłeś 21! Przegrałeś.");
        resultLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #FF4444;");
        currentBet = 0;
        state = GameState::FINISHED;
        setPlayingMode(false);
        updateBalance();
    }
}

void BlackjackGame::playerDoubleDown()
{
    if (!player->canBet(currentBet)) {
        QMessageBox::warning(this, "Brak środków", "Nie możesz podwoić zakładu!");
        return;
    }
    player->placeBet(currentBet);
    currentBet *= 2;
    emit balanceChanged();
    playerHand.append(deck.deal());
    updateCardsDisplay(playerCardsLayout, playerHand);
    playerScoreLabel->setText(QString("Punkty: %1").arg(calculateScore(playerHand)));
    playerStand();
}

void BlackjackGame::playerStand()
{
    // Dealer draws
    while (calculateScore(dealerHand) < 17) {
        dealerHand.append(deck.deal());
    }

    updateCardsDisplay(dealerCardsLayout, dealerHand);
    int ds = calculateScore(dealerHand);
    int ps = calculateScore(playerHand);
    dealerScoreLabel->setText(QString("Punkty: %1").arg(ds));

    checkResult();
}

void BlackjackGame::checkResult()
{
    int ps = calculateScore(playerHand);
    int ds = calculateScore(dealerHand);

    if (ds > 21 || ps > ds) {
        double win = currentBet * 2;
        player->addWinnings(win);
        resultLabel->setText(QString("🎉 WYGRAŁEŚ %1 zł!").arg(win, 0, 'f', 2));
        resultLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #00FF88;");
    } else if (ps == ds) {
        player->addWinnings(currentBet); // push — zwrot zakładu
        resultLabel->setText("🤝 Remis! Zakład zwrócony.");
        resultLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #FFD700;");
    } else {
        resultLabel->setText("😞 Przegrałeś! Krupier wygrał.");
        resultLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #FF4444;");
    }

    emit balanceChanged();
    currentBet = 0;
    state = GameState::FINISHED;
    setPlayingMode(false);
    updateBalance();
}

void BlackjackGame::setPlayingMode(bool playing)
{
    dealButton->setEnabled(!playing);
    hitButton->setEnabled(playing);
    standButton->setEnabled(playing);
    doubleButton->setEnabled(playing);
    betSpinBox->setEnabled(!playing);
}

void BlackjackGame::updateBalance()
{
    balanceLabel->setText(QString("Saldo: %1 zł").arg(player->getBalance(), 0, 'f', 2));
}

void BlackjackGame::resetGame()
{
    playerHand.clear();
    dealerHand.clear();
    updateCardsDisplay(playerCardsLayout, playerHand);
    updateCardsDisplay(dealerCardsLayout, dealerHand);
    playerScoreLabel->setText("Punkty: -");
    dealerScoreLabel->setText("Punkty: -");
    resultLabel->setText("");
    currentBet = 0;
    state = GameState::WAITING;
    setPlayingMode(false);
    updateBalance();
}
