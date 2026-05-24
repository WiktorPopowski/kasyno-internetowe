<<<<<<< HEAD
# Royal Flush Casino – Instrukcja uruchomienia

## Wymagania

- **Code::Blocks** (zalecana wersja: 20.03+ z kompilatorem MinGW)
  - Pobierz ze: https://www.codeblocks.org/downloads/
  - Wybierz wersję **codeblocks-XX.XX-setup-nonadmin.exe** (z MinGW w nazwie!)
  - MinGW zawiera kompilator GCC – bez niego projekt się nie skompiluje.

## Szybki start – Code::Blocks

1. Rozpakuj folder `RoyalFlushCasino` w dowolne miejsce.
2. Otwórz **Code::Blocks**.
3. `File → Open → wybierz plik RoyalFlushCasino.cbp`.
4. Naciśnij **F9** (Build and Run) lub zielony trójkąt ▶.
5. Gotowe – gra uruchomi się w oknie konsoli!

> **Uwaga:** Kolory ANSI działają w Windows 10/11 automatycznie.  
> W starszych wersjach Windows kolor może być wyłączony (tekst będzie
> czytelny, tylko bez kolorów).

## Struktura projektu

```
RoyalFlushCasino/
├── main.cpp                  ← punkt wejścia
├── RoyalFlushCasino.cbp      ← plik projektu Code::Blocks
├── README.md                 ← ta instrukcja
├── include/
│   ├── common.h              ← kolory ANSI, helpery UI
│   ├── player.h              ← klasa Player
│   ├── card.h                ← Card, Deck, Hand, BlackjackHand
│   ├── game.h                ← Game (baza) + deklaracje gier
│   └── casino.h              ← klasa Casino (fasada)
└── src/
    ├── player.cpp
    ├── card.cpp
    ├── blackjack.cpp         ← Blackjack (21) z 6 taliami
    ├── roulette.cpp          ← Ruletka Europejska
    ├── slots.cpp             ← Slot Machine (5 bębnów)
    ├── videopoker.cpp        ← Video Poker (Jacks or Better)
    ├── dice.cpp              ← Kości (Craps uproszczony)
    └── casino.cpp            ← główna logika aplikacji
```

## Opis gier

| Gra | Zasady | Wypłaty |
|-----|--------|---------|
| Blackjack | Dobierz do 21, nie przekrocz. BJ płaci 3:2 | 1:1 / 3:2 |
| Ruletka | Europejska (0–36). Wiele zakładów w turze | 1:1 do 35:1 |
| Slot Machine | 5 bębnów × 3 rzędy, 8 symboli | 5x–1000x zakład |
| Video Poker | Jacks or Better, trzymaj/wymień karty | 1x–800x |
| Kości (Craps) | Rzuć 7/11 = wygrana, 2/3/12 = przegrana | 1:1 |

## Diagram klas (skrót)

```
[Casino]
  ├── ma Player* m_player
  └── ma vector<Game*> m_games  ← polimorfizm

[Game] (klasa abstrakcyjna)
  ├── BlackjackGame  (dziedziczy Game)
  │     ├── używa Deck, BlackjackHand
  ├── RouletteGame   (dziedziczy Game)
  │     ├── używa RouletteBet (struct)
  ├── SlotMachine    (dziedziczy Game)
  │     └── używa Symbol (struct wewnętrzna)
  ├── VideoPoker     (dziedziczy Game)
  │     └── używa Deck, vector<Card>
  └── DiceGame       (dziedziczy Game)

[Card] ← Suit (enum), Rank (enum)
[Hand] (baza)
  └── BlackjackHand (dziedziczy Hand) → nadpisuje getValue(), print()

[Player]
  └── zawiera vector<TransactionRecord>
```

## Kompilacja ręczna (terminal / PowerShell)

```bash
g++ -std=c++14 -o casino main.cpp src/player.cpp src/card.cpp \
    src/blackjack.cpp src/roulette.cpp src/slots.cpp \
    src/videopoker.cpp src/dice.cpp src/casino.cpp
./casino
```

## Zapis postępu

Dane gracza zapisywane są automatycznie do pliku `<nick>.casino`
w folderze roboczym. Przy kolejnym logowaniu saldo i historia są
wczytywane automatycznie.
=======
# Kasyno Internetowe 
>>>>>>> f683d8a4f461dc3cb8e1797534b1518425ba67ba
