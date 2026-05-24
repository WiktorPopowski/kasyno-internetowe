#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <limits>
#include <map>
#include <cmath>

#ifdef _WIN32
    #include <windows.h>
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

// ============================================================
//  ANSI Color Codes (work on Windows 10+ and all UNIX)
// ============================================================
namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string DIM     = "\033[2m";

    // Foreground
    const std::string BLACK   = "\033[30m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    const std::string BRIGHT_RED     = "\033[91m";
    const std::string BRIGHT_GREEN   = "\033[92m";
    const std::string BRIGHT_YELLOW  = "\033[93m";
    const std::string BRIGHT_BLUE    = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN    = "\033[96m";
    const std::string BRIGHT_WHITE   = "\033[97m";

    // Background
    const std::string BG_BLACK   = "\033[40m";
    const std::string BG_RED     = "\033[41m";
    const std::string BG_GREEN   = "\033[42m";
    const std::string BG_YELLOW  = "\033[43m";
    const std::string BG_BLUE    = "\033[44m";
    const std::string BG_MAGENTA = "\033[45m";
    const std::string BG_CYAN    = "\033[46m";
    const std::string BG_WHITE   = "\033[47m";
}

// ============================================================
//  UI Helper Functions
// ============================================================
namespace UI {

inline void enableANSI() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    // Stare MinGW moze nie znac tej stalej – definiujemy recznie
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode))
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

inline void clearScreen() {
    system(CLEAR);
}

inline void pressEnter() {
    std::cout << "\n" << Color::DIM << Color::YELLOW
              << "  [ Nacisnij ENTER aby kontynuowac... ]"
              << Color::RESET << "\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

inline void printLine(char c = '-', int len = 60, const std::string& color = Color::BRIGHT_BLUE) {
    std::cout << color;
    for (int i = 0; i < len; i++) std::cout << c;
    std::cout << Color::RESET << "\n";
}

inline void printDoubleLine(int len = 60) {
    std::cout << Color::BRIGHT_YELLOW;
    for (int i = 0; i < len; i++) std::cout << '=';
    std::cout << Color::RESET << "\n";
}

inline void printCentered(const std::string& text, int width = 60,
                           const std::string& color = Color::BRIGHT_WHITE) {
    int padding = (width - (int)text.size()) / 2;
    if (padding < 0) padding = 0;
    std::cout << color;
    for (int i = 0; i < padding; i++) std::cout << ' ';
    std::cout << text << Color::RESET << "\n";
}

inline void printBox(const std::string& text, int width = 60,
                     const std::string& borderColor = Color::BRIGHT_YELLOW,
                     const std::string& textColor   = Color::BRIGHT_WHITE) {
    printDoubleLine(width);
    printCentered(text, width, textColor);
    printDoubleLine(width);
}

inline int getIntInput(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << Color::BRIGHT_CYAN << prompt << Color::RESET;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (value >= minVal && value <= maxVal) return value;
            std::cout << Color::BRIGHT_RED
                      << "  Nieprawidlowa wartosc! Zakres: ["
                      << minVal << " - " << maxVal << "]\n"
                      << Color::RESET;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << Color::BRIGHT_RED
                      << "  Nieprawidlowy format! Podaj liczbe.\n"
                      << Color::RESET;
        }
    }
}

inline double getDoubleInput(const std::string& prompt, double minVal, double maxVal) {
    double value;
    while (true) {
        std::cout << Color::BRIGHT_CYAN << prompt << Color::RESET;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (value >= minVal && value <= maxVal) return value;
            std::cout << Color::BRIGHT_RED
                      << "  Nieprawidlowa wartosc! Zakres: ["
                      << minVal << " - " << maxVal << "]\n"
                      << Color::RESET;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << Color::BRIGHT_RED
                      << "  Nieprawidlowy format! Podaj liczbe.\n"
                      << Color::RESET;
        }
    }
}

inline std::string getStringInput(const std::string& prompt) {
    std::string value;
    std::cout << Color::BRIGHT_CYAN << prompt << Color::RESET;
    std::getline(std::cin, value);
    return value;
}

} // namespace UI

#endif // COMMON_H
