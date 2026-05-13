#pragma once
#include <iostream>
#include <string>
#include <optional>
using namespace std;

class Board;

// ================================================================
//  Piece  (abstract base)
// ================================================================
class Piece {
protected:
    int    positionX;
    int    positionY;
    string colour;
    char   symbol;

public:
    Piece();
    Piece(string c, char s);
    virtual ~Piece() {}

    virtual int    getPositionX() = 0;
    virtual int    getPositionY() = 0;
    virtual void   setPosition(int x, int y) = 0;
    virtual string getColour() = 0;
    virtual char   getSymbol() = 0;
    virtual bool   isValidMove(int startX, int startY,
        int endX, int endY,
        Board& board) = 0;
};

// ================================================================
//  Board
// ================================================================
class Board {
    Piece* grid[8][8];
public:
    Board();
    ~Board();
    void    initializeBoard();
    void    setBoardWhite();
    void    setBoardBlack();
    void    displayBoard();
    Piece* getPiece(int x, int y);
    void    movePiece(int startX, int startY, int endX, int endY);
};

// ================================================================
//  Game  (owns the SFML window)
// ================================================================
class Game {
private:
    Board  board;
    bool   isWhiteTurn;
    bool   gameOver;

    // SFML members
    sf::RenderWindow window;
    sf::Font         font;
    int              selectedRow, selectedCol;
    bool             pieceSelected;
    string           statusMessage;

    // Colours
    sf::Color lightSquare{ 240, 217, 181 };
    sf::Color darkSquare{ 81,  42,  42 };
    sf::Color selectedClr{ 246, 246, 105, 200 };
    sf::Color bgColor{ 22,  21,  18 };

    static constexpr int TILE = 80;
    static constexpr int BOARD_OFFSET_X = 60;
    static constexpr int BOARD_OFFSET_Y = 40;

    void drawBoard();
    void drawPieces();
    void drawPiece(int row, int col, char sym, const string& colour);
    void drawStatus();
    void handleClick(int px, int py);
    void getValidMoves(int row, int col, bool validGrid[8][8]);

public:
    Game();
    void startGame();
};

