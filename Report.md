# ♟️ Chess Game — OOP C++ with GUI

> A fully functional two-player Chess Game built using **Object-Oriented Programming** in C++, featuring a graphical user interface powered by **SFML 3.x**.

---

## 👥 Group Members

| Name | GitHub |
|------|--------|
| Member 1 | [Muhammad Saad (F250662)](https://github.com/GhulamShabir95) |
| Member 2 | [Ghulam Shabir (F250785)](https://github.com/muhammadsaadcs) |
| Member 3 | [Ahmad Mujtaba (F250803)](https://github.com/MuhammadAhmadMujtaba) |

---

## 📋 Project Overview

This project was developed as part of the **Object-Oriented Programming** course at **NUCES Faisalabad - Chiniot Campus**. The goal was to design and implement a Chess Game that demonstrates core OOP principles including encapsulation, inheritance, polymorphism, and composition.

---

## 🎮 Features

- ✅ 8×8 graphical chessboard with colored squares and rank/file labels
- ✅ All 6 chess pieces: King, Queen, Rook, Bishop, Knight, Pawn
- ✅ Valid move enforcement for every piece
- ✅ Move highlighting (dots for empty squares, red ring for capturable pieces)
- ✅ Alternating turns — White moves first
- ✅ Game over detection when King is captured
- ✅ Clean SFML GUI with status messages

---

## 🧱 OOP Concepts Used

### 1. Encapsulation
Data members like `positionX`, `positionY`, `colour`, and `symbol` are declared as `protected` inside the `Piece` class. They are only accessible through getter/setter functions.

### 2. Inheritance
All 6 piece classes inherit from the abstract base class `Piece`:
```
Piece (abstract base class)
├── King
├── Queen
├── Rook
├── Bishop
├── Knight
└── Pawn
```

### 3. Polymorphism
`isValidMove()` is a **pure virtual function** in the `Piece` base class. Each derived class overrides it with its own movement logic.
```cpp
virtual bool isValidMove(int startX, int startY, int endX, int endY, Board& board) = 0;
```

### 4. Composition
- `Board` class contains a 2D array of `Piece*` objects (`grid[8][8]`)
- `Game` class contains a `Board` object

---

## 🗂️ Project Structure

```
Chess_With_GUI/
│
├── Chess.h           # All class declarations
├── Chess.cpp         # All class implementations
├── Chess_main.cpp    # Entry point (main function)
├── sfml-system-3.dll
├── sfml-window-3.dll
├── sfml-graphics-3.dll
└── README.md
```

---

## ⚙️ How to Run

### Requirements
- Visual Studio 2022
- SFML 3.x (linked in project properties)

### Steps
1. Clone the repository
2. Open `Chess_With_GUI.vcxproj` in Visual Studio
3. Make sure SFML DLL files are in the same folder as the `.exe`
4. Press **Ctrl + F5** to build and run

---

## 🎯 How to Play

1. Click on any of your pieces to select it
2. Valid moves will be highlighted with dots
3. Click a highlighted square to move
4. Capture the opponent's King to win!

---

## 📸 Screenshots

<img width="455" height="460" alt="image" src="https://github.com/user-attachments/assets/d0049279-aa68-42ad-a933-b01a88b29796" />
<img width="455" height="460" alt="image" src="https://github.com/user-attachments/assets/3cc41932-0f2f-4104-908b-aa52e3ac9297" />
<img width="455" height="460" alt="image" src="https://github.com/user-attachments/assets/f46f1dfb-e178-4853-8c6c-4d81d433c046" />



---

## 📚 Course Information

- **Course**: Object-Oriented Programming (OOP)
- **University**: National University of Computer & Emerging Sciences
- **Campus**: Faisalabad - Chiniot Campus

---



*Made with ♟️ by Ghulam Shabir -*
