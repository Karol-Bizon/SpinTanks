# SpinTanks
XD
## Aktualne mechaniki
- Dwa czołgi w widoku 2D
- Czołg:
  - obraca się stale z zadaną prędkością kątową
  - po przytrzymaniu klawisza jedzie prosto w aktualnym kierunku
  - po puszczeniu klawisza wraca do obrotu
  - po każdym puszczeniu klawisza zmienia kierunek obrotu
- Ograniczony obszar świata

## Do zrobienia 
- Mechanika strzelania (strzał po wciśnięciu, przegrzanie po zbyt dużej liczbie strzałów)
- Generowanie przeszkód
- Grafika
- Nagrody i kary (boost do prędkości, szybszy obrót itp.)

## Sterowanie
- **Czołg 1**:  
  - `Space`

- **Czołg 2**:  
  - `Enter`

## Struktura projektu
- `main` uruchamia grę
- `game` zarządza pętlą gry
- `tank` opisuje czołg

## Wymagania
- C++17
- SFML 2.5+
- CMake 3.20+

## Budowanie i Uruchamianie
```bash
mkdir -p build
cmake -S . -B build
cmake --build build -j
./build/spinTanks
```

## Instalacja SFML (Ubuntu) 
```bash
sudo apt update
sudo apt install build-essential libsfml-dev
```
