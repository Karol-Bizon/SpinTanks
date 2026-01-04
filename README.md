# SpinTanks

## Aktualne mechaniki
- Dwa czołgi w widoku 2D
- Każdy czołg:
  - obraca się stale z zadaną prędkością kątową
  - po przytrzymaniu klawisza jedzie prosto w aktualnym kierunku
  - po puszczeniu klawisza wraca do obrotu
  - po każdym puszczeniu klawisza zmienia kierunek obrotu
- Ograniczony obszar świata

### Sterowanie
- **Czołg 1**:  
  - `Space` – jedź prosto (trzymaj), puść aby wrócić do obrotu

- **Czołg 2**:  
  - `Enter` – jedź prosto (trzymaj), puść aby wrócić do obrotu

## Struktura projektu
- `main` uruchamia grę
- `Game` zarządza pętlą gry i wejściem
- `Tank` odpowiada za własny stan i zachowanie

## Wymagania
- C++17
- SFML 2.5+
- CMake 3.20+

## Budowanie
mkdir -p build \\
cmake -S . -B build \\
cmake --build build -j

# instalacja SFML 
sudo apt update \\
sudo apt install build-essential libsfml-dev
