#include "Board.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

const int TILE_SIZE = 24;

Board::~Board() {}

Board::Board(int a, int b, int height, int width, int mines)
    : row(a), column(b), height(height), width(width), mineCount(mines) {

    grid.resize(a, std::vector<std::vector<std::vector<int>>>(b,
        std::vector<std::vector<int>>(height, std::vector<int>(width, 0))));
    revealed.resize(a, std::vector<std::vector<std::vector<bool>>>(b,
        std::vector<std::vector<bool>>(height, std::vector<bool>(width, false))));
    flagged.resize(a, std::vector<std::vector<std::vector<bool>>>(b,
        std::vector<std::vector<bool>>(height, std::vector<bool>(width, false))));

    srand(static_cast<unsigned>(time(0)));
}

bool Board::isFlagged(int a, int b, int x, int y) const {
    if (a < 0 || a >= row || b < 0 || b >= column ||
        x < 0 || x >= width || y < 0 || y >= height)
        return false;

    return flagged[a][b][y][x];
}

int Board::getTileValue(int a, int b, int x, int y) const {
    if (a < 0 || a >= row || b < 0 || b >= column ||
        x < 0 || x >= width || y < 0 || y >= height)
        return 0;

    return grid[a][b][y][x];
}

bool Board::isRevealed(int a, int b, int x, int y) const {
    if (a < 0 || a >= row || b < 0 || b >= column ||
        x < 0 || x >= width || y < 0 || y >= height)
        return false;

    return revealed[a][b][y][x];
}

void Board::setTile(int a, int b, int x, int y, int value, bool revealedState, bool flaggedState) {
    if (a < 0 || a >= row || b < 0 || b >= column ||
        x < 0 || x >= width || y < 0 || y >= height)
        return;

    grid[a][b][y][x] = value;
    revealed[a][b][y][x] = revealedState;
    flagged[a][b][y][x] = flaggedState;
}

void Board::placeMines(int ca, int cb, int cx, int cy) {
    if (mineCount >= row * column * height * width) return;
    int count = 0;
    while (count < mineCount) {
        int a = rand() % row;
        int b = rand() % column;
        int x = rand() % width;
        int y = rand() % height;

        if (grid[a][b][y][x] == -1) continue;
        if (a == ca && b == cb && x == cx && y == cy) continue;

        grid[a][b][y][x] = -1;
        count++;
        for (int da = -1; da <= 1; ++da) {
            for (int db = -1; db <= 1; ++db) {
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        int na = a + da;
                        int nb = b + db;
                        int nx = x + dx;
                        int ny = y + dy;
                        if (na >= 0 && na < row && nb >= 0 && nb < column &&
                            nx >= 0 && nx < width && ny >= 0 && ny < height &&
                            grid[na][nb][ny][nx] != -1) {
                            grid[na][nb][ny][nx]++;
                        }
                    }
                }
            }
        }
    }
}

void Board::reveal(int a, int b, int x, int y) {
    if (a < 0 || a >= row || b < 0 || b >= column ||
        x < 0 || x >= width || y < 0 || y >= height) return;
    if (revealed[a][b][y][x] || flagged[a][b][y][x]) return;

    if (grid[a][b][y][x] == -1) {
        for (int da = 0; da < row; ++da) {
            for (int db = 0; db < column; ++db) {
                for (int dy = 0; dy < height; ++dy) {
                    for (int dx = 0; dx < width; ++dx) {
                        if (grid[da][db][dy][dx] == -1) {
                            revealed[da][db][dy][dx] = true;
                        }
                    }
                }
            }
        }
        gameOver = true;
        return;
    }

    revealed[a][b][y][x] = true;
    revealedCount++;

    checkGameWin();

    if (grid[a][b][y][x] == 0) {
        for (int da = -1; da <= 1; ++da) {
            for (int db = -1; db <= 1; ++db) {
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (da != 0 || db != 0 || dx != 0 || dy != 0) {
                            int na = a + da;
                            int nb = b + db;
                            int nx = x + dx;
                            int ny = y + dy;
                            if (na >= 0 && na < row && nb >= 0 && nb < column &&
                                nx >= 0 && nx < width && ny >= 0 && ny < height) {
                                reveal(na, nb, nx, ny);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Board::checkGameWin() {
    int totalCells = row * column * width * height;
    int safeCells = totalCells - mineCount;
    if (revealedCount == safeCells) {
        gameWon = true;
        for (int a = 0; a < row; a++) {
            for (int b = 0; b < column; b++) {
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        if (grid[a][b][y][x] == -1 && !flagged[a][b][y][x]) {
                            flagged[a][b][y][x] = true;
                        }
                    }
                }
            }
        }
    }
}

void Board::handleClick(int a, int b, int x, int y, bool leftClick) {
    if (a < 0 || a >= row || b < 0 || b >= column ||
        x < 0 || x >= width || y < 0 || y >= height) return;
    if (gameOver || gameWon) return;

    if (leftClick) {
        if (!board_generated) {
            placeMines(a, b, x, y);
            board_generated = true;
            if (grid[a][b][y][x] == -1) return;
        }

        if (grid[a][b][y][x] != -1 && revealed[a][b][y][x]) {
            int flag_count = 0;
            for (int da = -1; da <= 1; da++) {
                for (int db = -1; db <= 1; db++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            int na = a + da;
                            int nb = b + db;
                            int nx = x + dx;
                            int ny = y + dy;
                            if (na >= 0 && na < row && nb >= 0 && nb < column &&
                                nx >= 0 && nx < width && ny >= 0 && ny < height) {
                                if (flagged[na][nb][ny][nx]) flag_count++;
                            }
                        }
                    }
                }
            }
            if (grid[a][b][y][x] == flag_count) {
                for (int da = -1; da <= 1; da++) {
                    for (int db = -1; db <= 1; db++) {
                        for (int dy = -1; dy <= 1; dy++) {
                            for (int dx = -1; dx <= 1; dx++) {
                                int na = a + da;
                                int nb = b + db;
                                int nx = x + dx;
                                int ny = y + dy;
                                if (na >= 0 && na < row && nb >= 0 && nb < column &&
                                    nx >= 0 && nx < width && ny >= 0 && ny < height) {
                                    reveal(na, nb, nx, ny);
                                }
                            }
                        }
                    }
                }
            }
        }

        else reveal(a, b, x, y);
    }
    else if (!revealed[a][b][y][x]) flagged[a][b][y][x] = !flagged[a][b][y][x];
}

void Board::draw(sf::RenderWindow& window, sf::Texture& texture, float globalOffsetX, float globalOffsetY, int boardSpacing) {
    sf::Sprite tile(texture);

    for (int a = 0; a < row; a++) {
        for (int b = 0; b < column; b++) {
            float boardOffsetX = globalOffsetX + b * (width * TILE_SIZE + boardSpacing);
            float boardOffsetY = globalOffsetY + a * (height * TILE_SIZE + boardSpacing);

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int index = 82; // ukryte

                    if (revealed[a][b][y][x]) {
                        if (grid[a][b][y][x] == -1) {
                            index = 81; // mina
                        }
                        else {
                            index = grid[a][b][y][x]; // 0-80
                        }
                    }
                    else if (flagged[a][b][y][x]) {
                        index = 83; // flaga
                    }

                    tile.setTextureRect(sf::IntRect(index * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
                    tile.setPosition(boardOffsetX + x * TILE_SIZE, boardOffsetY + y * TILE_SIZE);
                    window.draw(tile);
                }
            }
        }
    }
}