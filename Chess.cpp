#include "Chess.h"

// ================================================================
//  Piece
// ================================================================
Piece::Piece()
    : positionX(-1), positionY(-1), colour("NOT SET"), symbol('N') {
}

Piece::Piece(string c, char s)
    : positionX(-1), positionY(-1), colour(c), symbol(s) {
}

// ================================================================
//  Board
// ================================================================
Board::Board() { initializeBoard(); }

Board::~Board() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (grid[i][j]) { delete grid[i][j]; grid[i][j] = nullptr; }
}

Piece* Board::getPiece(int x, int y) {
    if (x < 0 || x >= 8 || y < 0 || y >= 8) return nullptr;
    return grid[x][y];
}

void Board::initializeBoard() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            grid[i][j] = nullptr;
}

void Board::setBoardWhite() {
    grid[7][0] = new Rook("White");   grid[7][1] = new Knight("White");
    grid[7][2] = new Bishop("White"); grid[7][3] = new Queen("White");
    grid[7][4] = new King("White");   grid[7][5] = new Bishop("White");
    grid[7][6] = new Knight("White"); grid[7][7] = new Rook("White");
    for (int j = 0; j < 8; j++) grid[6][j] = new Pawn("White");
    for (int j = 0; j < 8; j++) {
        grid[7][j]->setPosition(7, j);
        grid[6][j]->setPosition(6, j);
    }
}

void Board::setBoardBlack() {
    grid[0][0] = new Rook("Black");   grid[0][1] = new Knight("Black");
    grid[0][2] = new Bishop("Black"); grid[0][3] = new Queen("Black");
    grid[0][4] = new King("Black");   grid[0][5] = new Bishop("Black");
    grid[0][6] = new Knight("Black"); grid[0][7] = new Rook("Black");
    for (int j = 0; j < 8; j++) grid[1][j] = new Pawn("Black");
    for (int j = 0; j < 8; j++) {
        grid[0][j]->setPosition(0, j);
        grid[1][j]->setPosition(1, j);
    }
}

void Board::displayBoard() { /* console fallback – not used in GUI mode */ }

void Board::movePiece(int sX, int sY, int eX, int eY) {
    if (sX < 0 || sX >= 8 || sY < 0 || sY >= 8 ||
        eX < 0 || eX >= 8 || eY < 0 || eY >= 8) return;

    Piece* p = grid[sX][sY];
    if (!p) return;
    if (grid[eX][eY] && grid[eX][eY]->getColour() == p->getColour()) return;
    if (grid[eX][eY]) delete grid[eX][eY];

    grid[eX][eY] = p;
    grid[sX][sY] = nullptr;
    p->setPosition(eX, eY);
}

// ================================================================
//  Game – constructor
// ================================================================
Game::Game()
    : isWhiteTurn(true), gameOver(false),
    selectedRow(-1), selectedCol(-1), pieceSelected(false),
    statusMessage("White's turn")
{
    // SFML 3.0: VideoMode takes sf::Vector2u{width, height}
    window.create(sf::VideoMode({ 760u, 760u }), "Chess Game",
        sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Try common Windows system fonts
    if (!font.openFromFile("C:/Windows/Fonts/segoeui.ttf"))
        font.openFromFile("C:/Windows/Fonts/arial.ttf");
}

// ================================================================
//  Game – draw the board squares + labels
// ================================================================
void Game::drawBoard() {
    window.clear(bgColor);

    const char files[] = "abcdefgh";

    for (int i = 0; i < 8; i++) {
        // Rank numbers on the left
        sf::Text rankTxt(font, to_string(8 - i), 16);  // SFML 3: (font, string, size)
        rankTxt.setFillColor({ 180, 160, 130 });
        rankTxt.setPosition({ static_cast<float>(BOARD_OFFSET_X - 22),
                              static_cast<float>(BOARD_OFFSET_Y + i * TILE + TILE / 2 - 10) });
        window.draw(rankTxt);

        // File letters on the bottom
        sf::Text fileTxt(font, string(1, files[i]), 16);
        fileTxt.setFillColor({ 180, 160, 130 });
        fileTxt.setPosition({ static_cast<float>(BOARD_OFFSET_X + i * TILE + TILE / 2 - 6),
                              static_cast<float>(BOARD_OFFSET_Y + 8 * TILE + 6) });
        window.draw(fileTxt);
    }

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            sf::RectangleShape sq({ static_cast<float>(TILE), static_cast<float>(TILE) });
            sq.setPosition({ static_cast<float>(BOARD_OFFSET_X + c * TILE),
                             static_cast<float>(BOARD_OFFSET_Y + r * TILE) });

            bool isLight = (r + c) % 2 == 0;
            sq.setFillColor(isLight ? lightSquare : darkSquare);
            window.draw(sq);

            // Highlight selected square
            if (pieceSelected && r == selectedRow && c == selectedCol) {
                sq.setFillColor(selectedClr);
                window.draw(sq);
            }
        }
    }
}

// ================================================================
//  Game – compute valid destinations for selected piece
// ================================================================
void Game::getValidMoves(int row, int col, bool validGrid[8][8]) {
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            validGrid[r][c] = false;
            Piece* p = board.getPiece(row, col);
            if (p && p->isValidMove(row, col, r, c, board))
                validGrid[r][c] = true;
        }
}

// ================================================================
//  Game – draw all pieces + valid-move indicators
// ================================================================
void Game::drawPieces() {
    bool validGrid[8][8] = {};
    if (pieceSelected) getValidMoves(selectedRow, selectedCol, validGrid);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (pieceSelected && validGrid[r][c]) {
                Piece* target = board.getPiece(r, c);
                if (target == nullptr) {
                    // Small dot on empty square
                    sf::CircleShape dot(10.f);
                    dot.setFillColor({ 0, 0, 0, 80 });
                    dot.setPosition({ static_cast<float>(BOARD_OFFSET_X + c * TILE + TILE / 2 - 10),
                                      static_cast<float>(BOARD_OFFSET_Y + r * TILE + TILE / 2 - 10) });
                    window.draw(dot);
                }
                else {
                    // Red ring on capturable piece
                    sf::RectangleShape ring({ static_cast<float>(TILE - 6), static_cast<float>(TILE - 6) });
                    ring.setPosition({ static_cast<float>(BOARD_OFFSET_X + c * TILE + 3),
                                       static_cast<float>(BOARD_OFFSET_Y + r * TILE + 3) });
                    ring.setFillColor(sf::Color::Transparent);
                    ring.setOutlineColor({ 200, 50, 50, 200 });
                    ring.setOutlineThickness(4.f);
                    window.draw(ring);
                }
            }

            Piece* p = board.getPiece(r, c);
            if (p) drawPiece(r, c, p->getSymbol(), p->getColour());
        }
    }
}

// ================================================================
//  Game – draw a single piece as a labelled circle
// ================================================================
void Game::drawPiece(int row, int col, char sym, const string& colour) {
    float cx = static_cast<float>(BOARD_OFFSET_X + col * TILE + TILE / 2);
    float cy = static_cast<float>(BOARD_OFFSET_Y + row * TILE + TILE / 2);

    // Drop shadow
    sf::CircleShape shadow(32.f);
    shadow.setOrigin({ 32.f, 32.f });
    shadow.setPosition({ cx + 2.f, cy + 3.f });
    shadow.setFillColor({ 0, 0, 0, 70 });
    window.draw(shadow);

    // Piece body
    sf::CircleShape circle(30.f);
    circle.setOrigin({ 30.f, 30.f });
    circle.setPosition({ cx, cy });

    bool isWhite = (colour == "White");
    circle.setFillColor(isWhite ? sf::Color{ 255, 255, 255 } : sf::Color{ 30, 30, 30 });
    circle.setOutlineColor(isWhite ? sf::Color{ 60, 60, 60 } : sf::Color{ 200, 200, 200 });
    circle.setOutlineThickness(2.f);
    window.draw(circle);

    // Letter label
    char upper = static_cast<char>(toupper(sym));
    string label;
    switch (upper) {
    case 'K': label = "K"; break;
    case 'Q': label = "Q"; break;
    case 'R': label = "R"; break;
    case 'B': label = "B"; break;
    case 'N': label = "N"; break;
    case 'P': label = "P"; break;
    default:  label = "?"; break;
    }

    // SFML 3.0: sf::Text constructor takes (font, string, characterSize)
    sf::Text txt(font, label, 28);
    txt.setStyle(sf::Text::Bold);
    txt.setFillColor(isWhite ? sf::Color{ 30, 30, 30 } : sf::Color{ 220, 220, 220 });

    sf::FloatRect lb = txt.getLocalBounds();
    // SFML 3.0: getLocalBounds() returns FloatRect with .position and .size
    txt.setOrigin({ lb.position.x + lb.size.x / 2.f,
                    lb.position.y + lb.size.y / 2.f });
    txt.setPosition({ cx, cy });
    window.draw(txt);
}

// ================================================================
//  Game – status bar at the top
// ================================================================
void Game::drawStatus() {
    sf::RectangleShape panel({ 760.f, 36.f });
    panel.setPosition({ 0.f, 0.f });
    panel.setFillColor({ 15, 15, 12 });
    window.draw(panel);

    sf::Text txt(font, statusMessage, 18);
    txt.setFillColor(gameOver ? sf::Color{ 220, 80, 80 } : sf::Color{ 210, 190, 140 });
    txt.setPosition({ static_cast<float>(BOARD_OFFSET_X), 8.f });
    window.draw(txt);

    // Coloured dot showing whose turn it is
    if (!gameOver) {
        sf::CircleShape dot(8.f);
        dot.setFillColor(isWhiteTurn ? sf::Color::White : sf::Color{ 30, 30, 30 });
        dot.setOutlineColor({ 180, 160, 130 });
        dot.setOutlineThickness(2.f);
        dot.setPosition({ static_cast<float>(BOARD_OFFSET_X - 30), 10.f });
        window.draw(dot);
    }
}

// ================================================================
//  Game – handle a left-click at pixel (px, py)
// ================================================================
void Game::handleClick(int px, int py) {
    if (gameOver) return;

    int col = (px - BOARD_OFFSET_X) / TILE;
    int row = (py - BOARD_OFFSET_Y) / TILE;
    if (col < 0 || col >= 8 || row < 0 || row >= 8) return;

    Piece* clicked = board.getPiece(row, col);

    if (!pieceSelected) {
        // ---- Select a piece belonging to the current player ----
        if (!clicked) return;
        if (isWhiteTurn && clicked->getColour() != "White") { statusMessage = "That's Black's piece!";  return; }
        if (!isWhiteTurn && clicked->getColour() != "Black") { statusMessage = "That's White's piece!"; return; }

        selectedRow = row;
        selectedCol = col;
        pieceSelected = true;
        statusMessage = (isWhiteTurn ? "White" : "Black");
        statusMessage += " — pick a destination";

    }
    else {
        // ---- A piece is already selected ----

        // Same square -> deselect
        if (row == selectedRow && col == selectedCol) {
            pieceSelected = false;
            statusMessage = isWhiteTurn ? "White's turn" : "Black's turn";
            return;
        }

        // Another friendly piece -> re-select
        if (clicked && clicked->getColour() == (isWhiteTurn ? "White" : "Black")) {
            selectedRow = row;
            selectedCol = col;
            statusMessage = (isWhiteTurn ? "White" : "Black");
            statusMessage += " — pick a destination";
            return;
        }

        Piece* moving = board.getPiece(selectedRow, selectedCol);
        if (!moving) { pieceSelected = false; return; }

        if (moving->isValidMove(selectedRow, selectedCol, row, col, board)) {
            Piece* target = board.getPiece(row, col);
            bool kingCaptured = target &&
                (target->getSymbol() == 'K' || target->getSymbol() == 'k');

            board.movePiece(selectedRow, selectedCol, row, col);
            pieceSelected = false;
            isWhiteTurn = !isWhiteTurn;

            if (kingCaptured) {
                gameOver = true;
                statusMessage = (isWhiteTurn ? "Black" : "White");
                statusMessage += " wins!  Close the window to exit.";
            }
            else {
                statusMessage = isWhiteTurn ? "White's turn" : "Black's turn";
            }
        }
        else {
            statusMessage = "Invalid move — try again.";
            pieceSelected = false;
        }
    }
}

// ================================================================
//  Game – main loop  (SFML 3.0 event API)
// ================================================================
void Game::startGame() {
    board.initializeBoard();
    board.setBoardWhite();
    board.setBoardBlack();
    statusMessage = "White's turn";

    while (window.isOpen()) {
        // ---- SFML 3.0 event loop ----
        // pollEvent() returns std::optional<sf::Event>; loop while it has a value
        while (const std::optional event = window.pollEvent()) {

            // Window close button
            if (event->is<sf::Event::Closed>())
                window.close();

            // ESC key  (SFML 3.0: scoped enum sf::Keyboard::Key::Escape)
            if (const auto* kp = event->getIf<sf::Event::KeyPressed>())
                if (kp->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();

            // Left mouse button click  (SFML 3.0: scoped enum sf::Mouse::Button::Left)
            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>())
                if (mb->button == sf::Mouse::Button::Left)
                    handleClick(mb->position.x, mb->position.y);
        }

        // ---- Render ----
        drawBoard();
        drawPieces();
        drawStatus();
        window.display();
    }
}

// ================================================================
//  PIECE IMPLEMENTATIONS  (game logic unchanged from original)
// ================================================================

// ---- King ----
King::King() : Piece() {}
King::King(string c) : Piece(c, (c == "White") ? 'K' : 'k') {}
string King::getColour() { return colour; }
int    King::getPositionX() { return positionX; }
int    King::getPositionY() { return positionY; }
void   King::setPosition(int x, int y) { positionX = x; positionY = y; }
char   King::getSymbol() { return symbol; }
bool   King::isValidMove(int sX, int sY, int eX, int eY, Board& board) {
    int dx = abs(eX - sX), dy = abs(eY - sY);
    if (dx == 0 && dy == 0) return false;
    if (dx > 1 || dy > 1) return false;
    Piece* t = board.getPiece(eX, eY);
    if (t && t->getColour() == colour) return false;
    return true;
}

// ---- Queen ----
Queen::Queen() : Piece() {}
Queen::Queen(string c) : Piece(c, (c == "White") ? 'Q' : 'q') {}
string Queen::getColour() { return colour; }
int    Queen::getPositionX() { return positionX; }
int    Queen::getPositionY() { return positionY; }
void   Queen::setPosition(int x, int y) { positionX = x; positionY = y; }
char   Queen::getSymbol() { return symbol; }
bool   Queen::isValidMove(int sX, int sY, int eX, int eY, Board& board) {
    int dx = eX - sX, dy = eY - sY;
    if (dx == 0 && dy == 0) return false;
    bool straight = (dx == 0 || dy == 0);
    bool diagonal = (abs(dx) == abs(dy));
    if (!straight && !diagonal) return false;
    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
    int x = sX + stepX, y = sY + stepY;
    while (x != eX || y != eY) {
        if (board.getPiece(x, y)) return false;
        x += stepX; y += stepY;
    }
    Piece* t = board.getPiece(eX, eY);
    if (t && t->getColour() == colour) return false;
    return true;
}

// ---- Rook ----
Rook::Rook() : Piece() {}
Rook::Rook(string c) : Piece(c, (c == "White") ? 'R' : 'r') {}
string Rook::getColour() { return colour; }
int    Rook::getPositionX() { return positionX; }
int    Rook::getPositionY() { return positionY; }
void   Rook::setPosition(int x, int y) { positionX = x; positionY = y; }
char   Rook::getSymbol() { return symbol; }
bool   Rook::isValidMove(int sX, int sY, int eX, int eY, Board& board) {
    int dx = eX - sX, dy = eY - sY;
    if (dx == 0 && dy == 0) return false;
    if (dx != 0 && dy != 0) return false;
    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
    int x = sX + stepX, y = sY + stepY;
    while (x != eX || y != eY) {
        if (board.getPiece(x, y)) return false;
        x += stepX; y += stepY;
    }
    Piece* t = board.getPiece(eX, eY);
    if (t && t->getColour() == colour) return false;
    return true;
}

// ---- Bishop ----
Bishop::Bishop() : Piece() {}
Bishop::Bishop(string c) : Piece(c, (c == "White") ? 'B' : 'b') {}
string Bishop::getColour() { return colour; }
int    Bishop::getPositionX() { return positionX; }
int    Bishop::getPositionY() { return positionY; }
void   Bishop::setPosition(int x, int y) { positionX = x; positionY = y; }
char   Bishop::getSymbol() { return symbol; }
bool   Bishop::isValidMove(int sX, int sY, int eX, int eY, Board& board) {
    int dx = eX - sX, dy = eY - sY;
    if (dx == 0 && dy == 0) return false;
    if (abs(dx) != abs(dy)) return false;
    int stepX = (dx > 0) ? 1 : -1;
    int stepY = (dy > 0) ? 1 : -1;
    int x = sX + stepX, y = sY + stepY;
    while (x != eX) {
        if (board.getPiece(x, y)) return false;
        x += stepX; y += stepY;
    }
    Piece* t = board.getPiece(eX, eY);
    if (t && t->getColour() == colour) return false;
    return true;
}

// ---- Knight ----
Knight::Knight() : Piece() {}
Knight::Knight(string c) : Piece(c, (c == "White") ? 'N' : 'n') {}
string Knight::getColour() { return colour; }
int    Knight::getPositionX() { return positionX; }
int    Knight::getPositionY() { return positionY; }
void   Knight::setPosition(int x, int y) { positionX = x; positionY = y; }
char   Knight::getSymbol() { return symbol; }
bool   Knight::isValidMove(int sX, int sY, int eX, int eY, Board& board) {
    int dx = abs(eX - sX), dy = abs(eY - sY);
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) return false;
    Piece* t = board.getPiece(eX, eY);
    if (t && t->getColour() == colour) return false;
    return true;
}

// ---- Pawn ----
Pawn::Pawn() : Piece() {}
Pawn::Pawn(string c) : Piece(c, (c == "White") ? 'P' : 'p') {}
string Pawn::getColour() { return colour; }
int    Pawn::getPositionX() { return positionX; }
int    Pawn::getPositionY() { return positionY; }
void   Pawn::setPosition(int x, int y) { positionX = x; positionY = y; }
char   Pawn::getSymbol() { return symbol; }
bool   Pawn::isValidMove(int sX, int sY, int eX, int eY, Board& board) {
    int direction = (colour == "White") ? -1 : 1;
    int startRow = (colour == "White") ? 6 : 1;
    int dx = eX - sX, dy = eY - sY;
    Piece* target = board.getPiece(eX, eY);

    // One step forward (empty square)
    if (dy == 0 && dx == direction && target == nullptr)
        return true;

    // Two steps from starting row (both squares empty)
    if (dy == 0 && dx == 2 * direction && sX == startRow &&
        target == nullptr &&
        board.getPiece(sX + direction, sY) == nullptr)
        return true;

    // Diagonal capture
    if (abs(dy) == 1 && dx == direction &&
        target != nullptr && target->getColour() != colour)
        return true;

    return false;
}