# 💣 Minesweeper Game (C + Raylib)

A classic Minesweeper game built using **C programming language** and the **Raylib graphics library**.  
This project recreates the traditional Minesweeper gameplay with a simple graphical interface.

---

## 🎮 Features

- Classic Minesweeper mechanics
- Grid-based gameplay
- Random mine placement
- Left click to reveal tiles
- Right click to flag mines
- Win/Lose detection
- Lightweight and fast (built with Raylib)

---

## 🛠️ Built With

- C (C99 standard)
- Raylib (graphics library)
- GCC compiler
  
---

## 🚀 How to Run

### 1. Install requirements
- GCC (MinGW / w64devkit)
- Raylib installed and configured

### 2. Compile

```bash
gcc main.c -o main.exe -O2 -Wall -std=c99 -IC:\raylib\raylib\src -L. -lraylib -lopengl32 -lgdi32 -lwinmm
```
### 3. Run
```bash
main.exe
