#include "../include/casino.h"

// ============================================================
//  Konstruktor / destruktor
// ============================================================
Casino::Casino() : m_player(nullptr), m_running(false) {
    srand((unsigned)time(nullptr));
    UI::enableANSI();
}

Casino::~Casino() {
    cleanup();
}

void Casino::cleanup() {
    for (Game* g : m_games) delete g;
    m_games.clear();
    if (m_player) {
        m_player->saveToFile();
        delete m_player;
        m_player = nullptr;
    }
}

// ============================================================
//  Rejestracja gier (polimorfizm przez wskaźniki)
// ============================================================
void Casino::registerGames() {
    // Czyści poprzednią listę (na wypadek ponownego logowania)
    for (Game* g : m_games) delete g;
    m_games.clear();

    m_games.push_back(new BlackjackGame(*m_player));
    m_games.push_back(new RouletteGame(*m_player));
    m_games.push_back(new SlotMachine(*m_player));
    m_games.push_back(new VideoPoker(*m_player));
    m_games.push_back(new DiceGame(*m_player));
}

// ============================================================
//  Ekrany
// ============================================================
void Casino::printWelcomeBanner() const {
    UI::clearScreen();
    std::cout << "\n\n";
    std::cout << Color::BRIGHT_YELLOW << Color::BOLD;
    std::cout << R"(
  ██████╗  ██████╗ ██╗   ██╗ █████╗ ██╗
  ██╔══██╗██╔═══██╗╚██╗ ██╔╝██╔══██╗██║
  ██████╔╝██║   ██║ ╚████╔╝ ███████║██║
  ██╔══██╗██║   ██║  ╚██╔╝  ██╔══██║██║
  ██║  ██║╚██████╔╝   ██║   ██║  ██║███████╗
  ╚═╝  ╚═╝ ╚═════╝    ╚═╝   ╚═╝  ╚═╝╚══════╝
)";
    std::cout << Color::BRIGHT_CYAN;
    std::cout << R"(
  ███████╗██╗     ██╗   ██╗███████╗██╗  ██╗
  ██╔════╝██║     ██║   ██║██╔════╝██║  ██║
  █████╗  ██║     ██║   ██║███████╗███████║
  ██╔══╝  ██║     ██║   ██║╚════██║██╔══██║
  ██║     ███████╗╚██████╔╝███████║██║  ██║
  ╚═╝     ╚══════╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝
)";
    std::cout << Color::RESET;
    std::cout << Color::BRIGHT_YELLOW;
    UI::printDoubleLine();
    UI::printCentered("CASINO ONLINE  v1.0  -  Graj odpowiedzialnie!", 60, Color::BRIGHT_WHITE);
    UI::printDoubleLine();
    std::cout << Color::RESET << "\n";
}

void Casino::printMainMenu() const {
    UI::clearScreen();
    printWelcomeBanner();
    std::cout << Color::BRIGHT_CYAN << "\n";
    UI::printCentered("WITAJ W KASYNIE", 60, Color::BRIGHT_YELLOW + Color::BOLD);
    std::cout << "\n";
    std::cout << "  [1]  Zaloguj sie (istniejace konto)\n"
              << "  [2]  Zarejestruj sie (nowe konto)\n"
              << "  [3]  Wyjdz z gry\n\n"
              << Color::RESET;
}

void Casino::printCasinoLobby() const {
    UI::clearScreen();
    std::cout << Color::BRIGHT_YELLOW << Color::BOLD;
    UI::printDoubleLine();
    UI::printCentered("*** ROYAL FLUSH CASINO ***  LOBBY", 60,
                      Color::BRIGHT_YELLOW + Color::BOLD);
    UI::printDoubleLine();
    std::cout << Color::RESET;
    m_player->printHeader();

    std::cout << Color::BRIGHT_CYAN << "  DOSTEPNE GRY:\n\n" << Color::RESET;
    for (int i = 0; i < (int)m_games.size(); ++i) {
        std::cout << Color::BRIGHT_WHITE << "  [" << (i+1) << "]  "
                  << Color::BRIGHT_GREEN << m_games[i]->getName()
                  << Color::RESET << "\n";
    }
    std::cout << "\n"
              << Color::BRIGHT_CYAN
              << "  [" << (m_games.size()+1) << "]  Kasa (Depozyt / Wyplata)\n"
              << "  [" << (m_games.size()+2) << "]  Moje statystyki\n"
              << "  [" << (m_games.size()+3) << "]  Historia gier\n"
              << "  [" << (m_games.size()+4) << "]  Wyloguj i zapisz\n"
              << Color::RESET << "\n";
}

void Casino::printCashierMenu() {
    UI::clearScreen();
    UI::printBox("KASA", 60);
    m_player->printHeader();

    std::cout << "  [1]  Wplac srodki\n"
              << "  [2]  Wyplac srodki\n"
              << "  [3]  Wróc do lobby\n\n";

    int choice = UI::getIntInput("  Twoj wybor: ", 1, 3);

    if (choice == 1) {
        double amount = UI::getDoubleInput("  Kwota wplaty (1-10000): ", 1.0, 10000.0);
        m_player->deposit(amount);
        std::cout << Color::BRIGHT_GREEN
                  << "\n  Wplacono " << amount << " zl. Nowe saldo: "
                  << m_player->getBalance() << " zl\n"
                  << Color::RESET;
        UI::pressEnter();
    } else if (choice == 2) {
        double maxW = m_player->getBalance();
        if (maxW < 1.0) {
            std::cout << Color::BRIGHT_RED << "\n  Brak srodkow do wyplaty.\n" << Color::RESET;
        } else {
            double amount = UI::getDoubleInput(
                "  Kwota wyplaty (1-" + std::to_string((int)maxW) + "): ",
                1.0, maxW);
            if (m_player->withdraw(amount))
                std::cout << Color::BRIGHT_GREEN
                          << "\n  Wyplacono " << amount << " zl. Nowe saldo: "
                          << m_player->getBalance() << " zl\n" << Color::RESET;
            else
                std::cout << Color::BRIGHT_RED << "\n  Blad wyplaty.\n" << Color::RESET;
        }
        UI::pressEnter();
    }
}

void Casino::printAccountInfo() const {
    UI::clearScreen();
    m_player->printStats();
    UI::pressEnter();
}

// ============================================================
//  Logowanie / Rejestracja
// ============================================================
bool Casino::loginOrRegister() {
    while (true) {
        printMainMenu();
        int choice = UI::getIntInput("  Twoj wybor: ", 1, 3);

        if (choice == 3) return false;

        std::string name = UI::getStringInput("  Podaj nick gracza: ");
        if (name.empty()) {
            std::cout << Color::BRIGHT_RED << "  Nick nie moze byc pusty!\n" << Color::RESET;
            UI::pressEnter();
            continue;
        }

        if (choice == 1) {
            // Logowanie
            Player* p = new Player(name);
            if (p->loadFromFile(name)) {
                m_player = p;
                std::cout << Color::BRIGHT_GREEN
                          << "\n  Witaj z powrotem, " << name << "!\n"
                          << "  Twoje saldo: " << p->getBalance() << " zl\n"
                          << Color::RESET;
                UI::pressEnter();
                return true;
            } else {
                delete p;
                std::cout << Color::BRIGHT_RED
                          << "\n  Nie znaleziono konta dla: " << name
                          << "\n  Czy chcesz zalozyc nowe? [1=Tak / 2=Nie]\n"
                          << Color::RESET;
                int c2 = UI::getIntInput("  > ", 1, 2);
                if (c2 == 1) {
                    m_player = new Player(name, 1000.0);
                    std::cout << Color::BRIGHT_GREEN
                              << "\n  Konto utworzone! Bonus startowy: 1000 zl\n"
                              << Color::RESET;
                    UI::pressEnter();
                    return true;
                }
            }
        } else {
            // Rejestracja
            m_player = new Player(name, 1000.0);
            std::cout << Color::BRIGHT_GREEN
                      << "\n  Konto zalezone dla: " << name
                      << "\n  Bonus powitalny: 1000 zl!\n"
                      << Color::RESET;
            UI::pressEnter();
            return true;
        }
    }
}

// ============================================================
//  Wybór gry
// ============================================================
void Casino::runGameSelection() {
    int maxOpt = (int)m_games.size() + 4;
    while (true) {
        printCasinoLobby();
        int choice = UI::getIntInput("  Twoj wybor: ", 1, maxOpt);

        if (choice <= (int)m_games.size()) {
            // Uruchom wybraną grę (polimorfizm – Game::run())
            m_games[choice - 1]->run();
        } else if (choice == (int)m_games.size() + 1) {
            printCashierMenu();
        } else if (choice == (int)m_games.size() + 2) {
            printAccountInfo();
        } else if (choice == (int)m_games.size() + 3) {
            UI::clearScreen();
            m_player->printHistory();
            UI::pressEnter();
        } else {
            // Wyloguj
            m_player->saveToFile();
            std::cout << Color::BRIGHT_YELLOW
                      << "\n  Profil zapisany. Do zobaczenia, "
                      << m_player->getName() << "!\n"
                      << Color::RESET;
            UI::pressEnter();
            return;
        }
    }
}

// ============================================================
//  Punkt wejścia – start()
// ============================================================
void Casino::start() {
    m_running = true;
    while (m_running) {
        printWelcomeBanner();
        bool logged = loginOrRegister();
        if (!logged) {
            std::cout << Color::BRIGHT_YELLOW
                      << "\n  Dziekujemy za gre. Do zobaczenia!\n"
                      << Color::RESET;
            break;
        }

        registerGames();
        runGameSelection();

        // Po wylogowaniu wyczyść gracza
        if (m_player) {
            delete m_player;
            m_player = nullptr;
        }
        for (Game* g : m_games) delete g;
        m_games.clear();

        // Zapytaj czy zalogować ponownie
        std::cout << "\n  [1] Zaloguj innego gracza  [2] Wyjdz\n";
        int c = UI::getIntInput("  > ", 1, 2);
        if (c == 2) m_running = false;
    }
}
