#include "Tile.h"

const int TILE_SIZE = 24;

Tile::Tile() : value(0), revealed(false), flagged(false), position(0, 0) {}

void Tile::setPosition(int x, int y) {
    position.x = x;
    position.y = y;
}

void Tile::setValue(int val) {
    value = val;
}

void Tile::setRevealed(bool rev) {
    revealed = rev;
}

void Tile::setFlagged(bool flag) {
    flagged = flag;
}

bool Tile::isRevealed() const {
    return revealed;
}

bool Tile::isFlagged() const {
    return flagged;
}

int Tile::getValue() const {
    return value;
}

void Tile::draw(sf::RenderWindow& window, sf::Texture& texture) {
    sf::Sprite sprite(texture);
    int tileIndex = 0;

    if (flagged) {
        tileIndex = 83; // flaga
    }
    else if (!revealed) {
        tileIndex = 82; // ukryte pole
    }
    else {
        if (value == -1) {
            tileIndex = 81; // mina
        }
        else if (value > 0) {
            tileIndex = value; // liczba od 1 do 80
        }
        else {
            tileIndex = 0; // puste odkryte pole (wartoœæ 0)
        }
    }

    sprite.setTextureRect(sf::IntRect(tileIndex * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
    sprite.setPosition(position);
    window.draw(sprite);
}