#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QFont>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("🎰 Royal Casino");
    setMinimumSize(900, 700);
    resize(1000, 750);

    player = new Player("Gracz", 1000.0);

    applyGlobalStyle();

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupLoginPage();
    setupLobbyPage();
    setupSlotPage();
    setupBlackjackPage();
    setupRoulettePage();
    setupHistoryPage();
    setupDepositPage();

    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(lobbyPage);
    stackedWidget->addWidget(slotPage);
    stackedWidget->addWidget(blackjackPage);
    stackedWidget->addWidget(roulettePage);
    stackedWidget->addWidget(historyPage);
    stackedWidget->addWidget(depositPage);

    stackedWidget->setCurrentWidget(loginPage);
}

MainWindow::~MainWindow()
{
    delete player;
}

void MainWindow::applyGlobalStyle()
{
    setStyleSheet(
        "QMainWindow { background: #0d0d1a; }"
        "QWidget { background: #0d0d1a; color: #DDDDDD; font-family: 'Segoe UI'; }"
        "QScrollArea { border: none; background: #0d0d1a; }"
        "QScrollBar:vertical { background: #1a1a2e; width: 8px; border-radius: 4px; }"
        "QScrollBar::handle:vertical { background: #444; border-radius: 4px; min-height: 20px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
    );
}

QWidget* MainWindow::createHeader(const QString &title, bool showBack)
{
    QFrame *header = new QFrame();
    header->setFixedHeight(60);
    header->setStyleSheet("QFrame { background: #0a0a16; border-bottom: 2px solid #FFD700; }");
    QHBoxLayout *hl = new QHBoxLayout(header);
    hl->setContentsMargins(15, 5, 15, 5);

    if (showBack) {
        QPushButton *back = new QPushButton("◀ Lobby");
        back->setFixedSize(100, 36);
        back->setStyleSheet("QPushButton { background: #222; color: #FFD700; border: 1px solid #FFD700; "
                             "border-radius: 5px; font-size: 13px; font-weight: bold; } "
                             "QPushButton:hover { background: #333; }");
        connect(back, &QPushButton::clicked, this, &MainWindow::showLobby);
        hl->addWidget(back);
    }

    QLabel *lbl = new QLabel(title);
    lbl->setStyleSheet("color: #FFD700; font-size: 18px; font-weight: bold; background: transparent;");
    lbl->setAlignment(Qt::AlignCenter);
    hl->addWidget(lbl, 1);

    headerBalanceLabel = new QLabel();
    headerBalanceLabel->setStyleSheet("color: #00FF88; font-size: 14px; font-weight: bold; background: transparent;");
    headerBalanceLabel->setText(QString("💰 %1 zł").arg(player->getBalance(), 0, 'f', 2));
    hl->addWidget(headerBalanceLabel);

    return header;
}

void MainWindow::updateBalanceDisplay()
{
    if (headerBalanceLabel)
        headerBalanceLabel->setText(QString("💰 %1 zł").arg(player->getBalance(), 0, 'f', 2));
}

// ===== LOGIN PAGE =====
void MainWindow::setupLoginPage()
{
    loginPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(loginPage);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(25);

    QLabel *logo = new QLabel("🎰");
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet("font-size: 80px; background: transparent;");
    layout->addWidget(logo);

    QLabel *name = new QLabel("ROYAL CASINO");
    name->setAlignment(Qt::AlignCenter);
    name->setStyleSheet("font-size: 42px; font-weight: bold; color: #FFD700; "
                        "letter-spacing: 6px; background: transparent;");
    layout->addWidget(name);

    QLabel *sub = new QLabel("Twoje szczęście czeka");
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("font-size: 16px; color: #888; letter-spacing: 2px; background: transparent;");
    layout->addWidget(sub);

    QFrame *sep = new QFrame();
    sep->setFixedWidth(300);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #333;");
    layout->addWidget(sep, 0, Qt::AlignCenter);

    QLabel *nameLbl = new QLabel("Twoje imię:");
    nameLbl->setAlignment(Qt::AlignCenter);
    nameLbl->setStyleSheet("color: #CCC; font-size: 15px; background: transparent;");
    layout->addWidget(nameLbl);

    playerNameEdit = new QLineEdit();
    playerNameEdit->setPlaceholderText("Wpisz swoje imię...");
    playerNameEdit->setFixedSize(280, 42);
    playerNameEdit->setAlignment(Qt::AlignCenter);
    playerNameEdit->setText("Gracz");
    playerNameEdit->setStyleSheet("QLineEdit { background: #1a1a2e; color: #FFD700; "
                                   "border: 2px solid #FFD700; border-radius: 8px; "
                                   "padding: 8px; font-size: 15px; }");
    layout->addWidget(playerNameEdit, 0, Qt::AlignCenter);

    QPushButton *loginBtn = new QPushButton("🎰  WEJDŹ DO KASYNA");
    loginBtn->setFixedSize(280, 55);
    loginBtn->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #FFD700, stop:1 #FF6B35); color: #0d0d1a; font-size: 17px; "
        "font-weight: bold; border-radius: 27px; border: none; }"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #FF6B35, stop:1 #FFD700); }");
    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::handleLogin);
    connect(playerNameEdit, &QLineEdit::returnPressed, this, &MainWindow::handleLogin);
    layout->addWidget(loginBtn, 0, Qt::AlignCenter);

    QLabel *bonus = new QLabel("🎁 Bonus powitalny: 1000 zł na start!");
    bonus->setAlignment(Qt::AlignCenter);
    bonus->setStyleSheet("color: #00FF88; font-size: 13px; background: transparent;");
    layout->addWidget(bonus);
}

void MainWindow::handleLogin()
{
    QString name = playerNameEdit->text().trimmed();
    if (name.isEmpty()) name = "Gracz";
    delete player;
    player = new Player(name, 1000.0);

    // Re-assign player to games
    slotGame->~SlotMachine();
    new(slotGame) SlotMachine(player, nullptr);

    showLobby();
}

// ===== LOBBY =====
void MainWindow::setupLobbyPage()
{
    lobbyPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(lobbyPage);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    // Header
    QFrame *header = new QFrame();
    header->setFixedHeight(70);
    header->setStyleSheet("QFrame { background: #0a0a16; border-bottom: 2px solid #FFD700; }");
    QHBoxLayout *hl = new QHBoxLayout(header);
    hl->setContentsMargins(20, 5, 20, 5);

    QLabel *logoLbl = new QLabel("🎰 ROYAL CASINO");
    logoLbl->setStyleSheet("font-size: 22px; font-weight: bold; color: #FFD700; background: transparent;");
    hl->addWidget(logoLbl);
    hl->addStretch();

    balanceLabel = new QLabel();
    balanceLabel->setStyleSheet("font-size: 16px; color: #00FF88; font-weight: bold; background: transparent;");
    hl->addWidget(balanceLabel);

    QPushButton *depositBtn = new QPushButton("💳 Wpłata");
    depositBtn->setFixedSize(110, 38);
    depositBtn->setStyleSheet("QPushButton { background: #1a6b3a; color: white; border-radius: 6px; "
                               "font-size: 13px; font-weight: bold; } "
                               "QPushButton:hover { background: #2a8a4a; }");
    connect(depositBtn, &QPushButton::clicked, this, &MainWindow::showDeposit);
    hl->addWidget(depositBtn);

    QPushButton *histBtn = new QPushButton("📋 Historia");
    histBtn->setFixedSize(110, 38);
    histBtn->setStyleSheet("QPushButton { background: #1a3a6b; color: white; border-radius: 6px; "
                            "font-size: 13px; font-weight: bold; } "
                            "QPushButton:hover { background: #2a4a8b; }");
    connect(histBtn, &QPushButton::clicked, this, &MainWindow::showHistory);
    hl->addWidget(histBtn);

    layout->addWidget(header);

    // Content
    QWidget *content = new QWidget();
    QVBoxLayout *cl = new QVBoxLayout(content);
    cl->setAlignment(Qt::AlignTop);
    cl->setSpacing(30);
    cl->setContentsMargins(40, 30, 40, 30);

    QLabel *welcome = new QLabel("Wybierz grę");
    welcome->setAlignment(Qt::AlignCenter);
    welcome->setStyleSheet("font-size: 28px; color: #CCCCCC; letter-spacing: 2px;");
    cl->addWidget(welcome);

    // Game cards
    QHBoxLayout *gamesRow = new QHBoxLayout();
    gamesRow->setSpacing(25);

    auto makeGameCard = [&](const QString &emoji, const QString &name,
                             const QString &desc, const QString &color,
                             std::function<void()> slot) {
        QFrame *card = new QFrame();
        card->setFixedSize(230, 280);
        card->setStyleSheet(QString("QFrame { background: #12122a; border: 2px solid %1; "
                                     "border-radius: 18px; } "
                                     "QFrame:hover { background: #1a1a3a; border-color: %1; }").arg(color));
        QVBoxLayout *cl2 = new QVBoxLayout(card);
        cl2->setAlignment(Qt::AlignCenter);
        cl2->setSpacing(12);

        QLabel *emojiLbl = new QLabel(emoji);
        emojiLbl->setAlignment(Qt::AlignCenter);
        emojiLbl->setStyleSheet("font-size: 58px; background: transparent;");
        cl2->addWidget(emojiLbl);

        QLabel *nameLbl2 = new QLabel(name);
        nameLbl2->setAlignment(Qt::AlignCenter);
        nameLbl2->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1; "
                                        "background: transparent;").arg(color));
        cl2->addWidget(nameLbl2);

        QLabel *descLbl = new QLabel(desc);
        descLbl->setAlignment(Qt::AlignCenter);
        descLbl->setWordWrap(true);
        descLbl->setStyleSheet("font-size: 12px; color: #888; background: transparent;");
        cl2->addWidget(descLbl);

        QPushButton *btn = new QPushButton("ZAGRAJ");
        btn->setFixedSize(150, 40);
        btn->setStyleSheet(QString(
            "QPushButton { background: %1; color: white; border-radius: 20px; "
            "font-size: 14px; font-weight: bold; border: none; } "
            "QPushButton:hover { opacity: 0.85; }").arg(color));
        QObject::connect(btn, &QPushButton::clicked, slot);
        cl2->addWidget(btn, 0, Qt::AlignCenter);

        return card;
    };

    gamesRow->addWidget(makeGameCard("🎰", "Slot Machine",
        "Kręć bębnami i traf jackpota!\nDo 20x zakładu", "#FFD700",
        [this]{ showSlotMachine(); }));

    gamesRow->addWidget(makeGameCard("🃏", "Blackjack",
        "21 — pobij krupiera!\nDouble Down, strategia", "#00FF88",
        [this]{ showBlackjack(); }));

    gamesRow->addWidget(makeGameCard("🎡", "Ruletka",
        "Europejska ruletka.\nWiele typów zakładów", "#FF6B35",
        [this]{ showRoulette(); }));

    cl->addLayout(gamesRow);

    // Stats bar
    QFrame *statsBar = new QFrame();
    statsBar->setFixedHeight(70);
    statsBar->setStyleSheet("QFrame { background: #12122a; border: 1px solid #333; border-radius: 12px; }");
    QHBoxLayout *sl = new QHBoxLayout(statsBar);
    sl->setContentsMargins(20, 10, 20, 10);

    QLabel *s1 = new QLabel("🎮  3 gry dostępne");
    s1->setStyleSheet("color: #AAA; font-size: 13px; background: transparent;");
    sl->addWidget(s1);
    sl->addStretch();

    QLabel *s2 = new QLabel("⚡ Natychmiastowe wypłaty");
    s2->setStyleSheet("color: #AAA; font-size: 13px; background: transparent;");
    sl->addWidget(s2);
    sl->addStretch();

    QLabel *s3 = new QLabel("🔒 Bezpieczna gra");
    s3->setStyleSheet("color: #AAA; font-size: 13px; background: transparent;");
    sl->addWidget(s3);

    cl->addWidget(statsBar);
    layout->addWidget(content);
}

// ===== GAME PAGES =====
void MainWindow::setupSlotPage()
{
    slotPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(slotPage);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(createHeader("🎰 Slot Machine"));

    slotGame = new SlotMachine(player, slotPage);
    connect(slotGame, &SlotMachine::balanceChanged, this, &MainWindow::updateBalanceDisplay);
    connect(slotGame, &SlotMachine::balanceChanged, [this](){
        balanceLabel->setText(QString("💰 %1 zł").arg(player->getBalance(), 0, 'f', 2));
    });

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidget(slotGame);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    layout->addWidget(scroll);
}

void MainWindow::setupBlackjackPage()
{
    blackjackPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(blackjackPage);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(createHeader("🃏 Blackjack"));

    blackjackGame = new BlackjackGame(player, blackjackPage);
    connect(blackjackGame, &BlackjackGame::balanceChanged, this, &MainWindow::updateBalanceDisplay);
    connect(blackjackGame, &BlackjackGame::balanceChanged, [this](){
        balanceLabel->setText(QString("💰 %1 zł").arg(player->getBalance(), 0, 'f', 2));
    });

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidget(blackjackGame);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    layout->addWidget(scroll);
}

void MainWindow::setupRoulettePage()
{
    roulettePage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(roulettePage);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(createHeader("🎡 Ruletka Europejska"));

    rouletteGame = new RouletteGame(player, roulettePage);
    connect(rouletteGame, &RouletteGame::balanceChanged, this, &MainWindow::updateBalanceDisplay);
    connect(rouletteGame, &RouletteGame::balanceChanged, [this](){
        balanceLabel->setText(QString("💰 %1 zł").arg(player->getBalance(), 0, 'f', 2));
    });

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidget(rouletteGame);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    layout->addWidget(scroll);
}

void MainWindow::setupHistoryPage()
{
    historyPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(historyPage);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(createHeader("📋 Historia Transakcji"));

    historyTable = new QTableWidget();
    historyTable->setColumnCount(4);
    historyTable->setHorizontalHeaderLabels({"Czas", "Typ", "Kwota (zł)", "Saldo po (zł)"});
    historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setAlternatingRowColors(true);
    historyTable->setStyleSheet(
        "QTableWidget { background: #0d0d1a; color: #CCC; gridline-color: #333; border: none; font-size: 13px; }"
        "QHeaderView::section { background: #1a1a2e; color: #FFD700; padding: 8px; border: none; font-weight: bold; }"
        "QTableWidget::item:alternate { background: #121228; }"
        "QTableWidget::item:selected { background: #2a2a4a; }");

    layout->addWidget(historyTable);
}

void MainWindow::setupDepositPage()
{
    depositPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(depositPage);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(createHeader("💳 Wpłata środków"));

    QWidget *content = new QWidget();
    QVBoxLayout *cl = new QVBoxLayout(content);
    cl->setAlignment(Qt::AlignCenter);
    cl->setSpacing(20);

    QLabel *icon = new QLabel("💰");
    icon->setAlignment(Qt::AlignCenter);
    icon->setStyleSheet("font-size: 60px;");
    cl->addWidget(icon);

    QLabel *lbl = new QLabel("Wpłać środki na swoje konto");
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setStyleSheet("font-size: 20px; color: #CCC;");
    cl->addWidget(lbl);

    QLabel *amtLbl = new QLabel("Kwota wpłaty (zł):");
    amtLbl->setAlignment(Qt::AlignCenter);
    amtLbl->setStyleSheet("font-size: 15px; color: #AAA;");
    cl->addWidget(amtLbl);

    depositAmountEdit = new QLineEdit();
    depositAmountEdit->setPlaceholderText("Wpisz kwotę...");
    depositAmountEdit->setFixedSize(250, 45);
    depositAmountEdit->setAlignment(Qt::AlignCenter);
    depositAmountEdit->setStyleSheet("QLineEdit { background: #1a1a2e; color: #FFD700; "
                                      "border: 2px solid #FFD700; border-radius: 8px; "
                                      "padding: 8px; font-size: 16px; }");
    cl->addWidget(depositAmountEdit, 0, Qt::AlignCenter);

    // Quick amounts
    QHBoxLayout *quickRow = new QHBoxLayout();
    quickRow->setAlignment(Qt::AlignCenter);
    quickRow->setSpacing(10);
    for (int amt : {100, 250, 500, 1000}) {
        QPushButton *qb = new QPushButton(QString("+%1 zł").arg(amt));
        qb->setFixedSize(95, 38);
        qb->setStyleSheet("QPushButton { background: #1a1a2e; color: #FFD700; "
                           "border: 1px solid #FFD700; border-radius: 8px; font-size: 13px; } "
                           "QPushButton:hover { background: #2a2a4e; }");
        connect(qb, &QPushButton::clicked, [this, amt](){
            depositAmountEdit->setText(QString::number(amt));
        });
        quickRow->addWidget(qb);
    }
    cl->addLayout(quickRow);

    QPushButton *depBtn = new QPushButton("💳  WPŁAĆ");
    depBtn->setFixedSize(250, 52);
    depBtn->setStyleSheet("QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                           "stop:0 #1a6b3a, stop:1 #00FF88); color: white; font-size: 18px; "
                           "font-weight: bold; border-radius: 26px; border: none; } "
                           "QPushButton:hover { background: #2a8b5a; }");
    connect(depBtn, &QPushButton::clicked, this, &MainWindow::processDeposit);
    cl->addWidget(depBtn, 0, Qt::AlignCenter);

    layout->addWidget(content);
}

// ===== NAVIGATION =====
void MainWindow::showLobby()
{
    balanceLabel->setText(QString("💰 %1 zł").arg(player->getBalance(), 0, 'f', 2));
    stackedWidget->setCurrentWidget(lobbyPage);
}

void MainWindow::showSlotMachine()
{
    slotGame->resetGame();
    updateBalanceDisplay();
    stackedWidget->setCurrentWidget(slotPage);
}

void MainWindow::showBlackjack()
{
    blackjackGame->resetGame();
    updateBalanceDisplay();
    stackedWidget->setCurrentWidget(blackjackPage);
}

void MainWindow::showRoulette()
{
    rouletteGame->resetGame();
    updateBalanceDisplay();
    stackedWidget->setCurrentWidget(roulettePage);
}

void MainWindow::showHistory()
{
    const auto &hist = player->getHistory();
    historyTable->setRowCount(hist.size());
    for (int i = 0; i < hist.size(); i++) {
        const Transaction &t = hist[hist.size() - 1 - i]; // newest first
        historyTable->setItem(i, 0, new QTableWidgetItem(t.timestamp.toString("hh:mm:ss")));
        historyTable->setItem(i, 1, new QTableWidgetItem(t.type));
        auto amtItem = new QTableWidgetItem(QString("%1%2").arg(t.amount >= 0 ? "+" : "").arg(t.amount, 0, 'f', 2));
        amtItem->setForeground(t.amount >= 0 ? QColor("#00FF88") : QColor("#FF4444"));
        historyTable->setItem(i, 2, amtItem);
        historyTable->setItem(i, 3, new QTableWidgetItem(QString("%1 zł").arg(t.balanceAfter, 0, 'f', 2)));
    }
    updateBalanceDisplay();
    stackedWidget->setCurrentWidget(historyPage);
}

void MainWindow::showDeposit()
{
    depositAmountEdit->clear();
    updateBalanceDisplay();
    stackedWidget->setCurrentWidget(depositPage);
}

void MainWindow::processDeposit()
{
    bool ok;
    double amount = depositAmountEdit->text().toDouble(&ok);
    if (!ok || amount <= 0) {
        QMessageBox::warning(this, "Błąd", "Podaj poprawną kwotę.");
        return;
    }
    player->deposit(amount);
    balanceLabel->setText(QString("💰 %1 zł").arg(player->getBalance(), 0, 'f', 2));
    updateBalanceDisplay();
    QMessageBox::information(this, "Sukces",
                             QString("Wpłacono %1 zł!\nAktualne saldo: %2 zł")
                             .arg(amount, 0, 'f', 2)
                             .arg(player->getBalance(), 0, 'f', 2));
    showLobby();
}
