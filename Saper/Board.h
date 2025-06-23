#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <SFML/Graphics.hpp>

class Board {
public:
    Board(int a, int b, int height, int width, int mines);
    ~Board();

    void handleClick(int a, int b, int x, int y, bool leftClick);
    void draw(sf::RenderWindow& window, sf::Texture& texture, float globalOffsetX, float globalOffsetY, int boardSpacing);

    int getA() const { return row; }
    int getB() const { return column; }
    int getHeight() const { return height; }
    int getWidth() const { return width; }
    bool isGameOver() const { return gameOver; }
    bool isGameWon() const { return gameWon; }
    bool isFlagged(int a, int b, int x, int y) const;
    int getTileValue(int a, int b, int x, int y) const;
    bool isRevealed(int a, int b, int x, int y) const;
    int getRevealedCount() const { return revealedCount; }

    void setTile(int a, int b, int x, int y, int value, bool revealed, bool flagged);
    void setBoardGenerated(bool generated) { board_generated = generated; }
    void setGameOver(bool over) { gameOver = over; }
    void setGameWon(bool won) { gameWon = won; }
    void setRevealedCount(int count) { revealedCount = count; }

private:
    int row, column;
    int height, width;
    int mineCount;
    bool gameOver = false;
    bool gameWon = false;
    int revealedCount = 0;
    bool board_generated = false;

    std::vector<std::vector<std::vector<std::vector<int>>>> grid;
    std::vector<std::vector<std::vector<std::vector<bool>>>> revealed;
    std::vector<std::vector<std::vector<std::vector<bool>>>> flagged;

    void placeMines(int ca, int cb, int cx, int cy);
    void reveal(int a, int b, int x, int y);
    void checkGameWin();
};

#endif