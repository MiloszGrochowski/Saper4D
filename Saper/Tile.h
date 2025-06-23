#pragma once
#include <SFML/Graphics.hpp>

class Tile {
public:
    Tile();
    void setPosition(int x, int y);
    void setValue(int val);
    void setRevealed(bool rev);
    void setFlagged(bool flag);
    bool isRevealed() const;
    bool isFlagged() const;
    int getValue() const;
    void draw(sf::RenderWindow& window, sf::Texture& texture);

private:
    int value;
    bool revealed;
    bool flagged;
    sf::Vector2f position;
};