#include <SFML/Graphics.hpp>
#include "Board.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <vector>
#include <windows.h>
#include <algorithm>

std::vector<std::string> getSaveFiles() {
    std::vector<std::string> files;
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile("saves\\*.save", &findData);
    std::string folder = "saves\\";

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                files.push_back(folder + findData.cFileName);
            }
        } while (FindNextFile(hFind, &findData));
        FindClose(hFind);
    }
    return files;
}

int main() {
    int BOARD_ROWS = 5;
    int BOARD_COLS = 5;
    int WIDTH = 5;
    int HEIGHT = 5;
    int MINES = 40;
    const int TILE_SIZE = 24;
    const int SPACING = 10;

    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "Saper", sf::Style::Fullscreen);
    sf::Texture tilesTexture;
    if (!tilesTexture.loadFromFile("sprites/Tileset.png")) return -1;
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) return -1;

    Board* board = new Board(BOARD_ROWS, BOARD_COLS, HEIGHT, WIDTH, MINES);

    sf::Sprite resetButton(tilesTexture);
    resetButton.setTextureRect(sf::IntRect(84 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
    resetButton.setPosition(static_cast<float>(window.getSize().x) / 2 - TILE_SIZE / 2, 20.0f);

    bool timerStarted = false;
    sf::Clock gameClock;
    int elapsedSeconds = 0;
    int flagsPlaced = 0;

    sf::Text bombCounterText;
    sf::Text timerText;
    bombCounterText.setFont(font);
    bombCounterText.setCharacterSize(24);
    bombCounterText.setFillColor(sf::Color::White);
    bombCounterText.setPosition(1600.0f, 20.0f);

    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(1750.0f, 20.0f);

    sf::RectangleShape saveButton(sf::Vector2f(100.0f, 40.0f));
    saveButton.setPosition(static_cast<float>(window.getSize().x) - 150.0f, 70.0f);
    saveButton.setFillColor(sf::Color(100, 250, 50));
    sf::Text saveText("Save", font, 24);
    saveText.setPosition(saveButton.getPosition().x + 25.0f, saveButton.getPosition().y + 5.0f);
    saveText.setFillColor(sf::Color::Black);

    sf::RectangleShape loadButton(sf::Vector2f(100.0f, 40.0f));
    loadButton.setPosition(static_cast<float>(window.getSize().x) - 150.0f, 120.0f);
    loadButton.setFillColor(sf::Color(50, 150, 250));
    sf::Text loadText("Load", font, 24);
    loadText.setPosition(loadButton.getPosition().x + 25.0f, loadButton.getPosition().y + 5.0f);
    loadText.setFillColor(sf::Color::Black);

    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(24);
    infoText.setFillColor(sf::Color::Black);
    infoText.setPosition(static_cast<float>(window.getSize().x) / 2 - 100.0f, static_cast<float>(window.getSize().y) / 2);
    bool showInfo = false;
    sf::Clock infoClock;

    std::vector<sf::RectangleShape> inputBoxes;
    std::vector<sf::Text> inputLabels;
    std::vector<sf::Text> inputTexts;
    std::vector<std::string*> inputValues = {
        new std::string("5"), // Rows
        new std::string("5"), // Columns
        new std::string("5"), // Width
        new std::string("5"), // Height
        new std::string("40") // Mines
    };

    std::vector<std::string> labelStrs = {
        "Rows:", "Columns:", "Width:", "Height:", "Mines:"
    };

    for (int i = 0; i < 5; i++) {

        sf::Text label(labelStrs[i], font, 24);
        label.setPosition(50.0f, 30.0f + i * 40.0f);
        label.setFillColor(sf::Color::White);
        inputLabels.push_back(label);

        sf::RectangleShape box(sf::Vector2f(100.0f, 30.0f));
        box.setPosition(160.0f, 30.0f + i * 40.0f);
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color(150, 150, 150));
        inputBoxes.push_back(box);

        sf::Text text(*inputValues[i], font, 24);
        text.setPosition(165.0f, 30.0f + i * 40.0f);
        text.setFillColor(sf::Color::Black);
        inputTexts.push_back(text);
    }
    int activeInput = -1;
    bool needsReset = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (resetButton.getGlobalBounds().contains(mousePos)) {
                    needsReset = true;
                }

                if (saveButton.getGlobalBounds().contains(mousePos)) {
                    std::time_t now = std::time(nullptr);
                    std::tm* now_tm = std::localtime(&now);
                    char buffer[80];
                    std::strftime(buffer, sizeof(buffer), "saper_%Y-%m-%d-%H-%M-%S.save", now_tm);
                    std::string filename = buffer;

                    std::ofstream saveFile("saves/" + filename);
                    if (saveFile.is_open()) {
                        saveFile << BOARD_ROWS << " " << BOARD_COLS << " "
                            << WIDTH << " " << HEIGHT << " " << MINES << "\n";

                        saveFile << elapsedSeconds << " " << flagsPlaced << " "
                            << (timerStarted ? 1 : 0) << "\n";

                        saveFile << (board->isGameOver() ? 1 : 0) << " "
                            << (board->isGameWon() ? 1 : 0) << " "
                            << board->getRevealedCount() << "\n";

                        for (int a = 0; a < BOARD_ROWS; a++) {
                            for (int b = 0; b < BOARD_COLS; b++) {
                                for (int y = 0; y < HEIGHT; y++) {
                                    for (int x = 0; x < WIDTH; x++) {
                                        saveFile << board->getTileValue(a, b, x, y) << " "
                                            << (board->isRevealed(a, b, x, y) ? 1 : 0) << " "
                                            << (board->isFlagged(a, b, x, y) ? 1 : 0) << "\n";
                                    }
                                }
                            }
                        }
                        saveFile.close();

                        infoText.setString("Game saved to: " + filename);
                        showInfo = true;
                        infoClock.restart();
                    }
                }

                if (loadButton.getGlobalBounds().contains(mousePos)) {
                    sf::RenderWindow loadDialog(sf::VideoMode(600, 400), "Load Game");
                    std::vector<std::string> saveFiles = getSaveFiles();
                    std::vector<sf::Text> fileTexts;

                    for (size_t i = 0; i < saveFiles.size(); i++) {
                        sf::Text text(saveFiles[i], font, 20);
                        text.setPosition(50.0f, 50.0f + static_cast<float>(i) * 30.0f);
                        fileTexts.push_back(text);
                    }

                    sf::Text cancelText("Cancel", font, 20);
                    cancelText.setPosition(50.0f, 350.0f);
                    fileTexts.push_back(cancelText);

                    while (loadDialog.isOpen()) {
                        sf::Event dialogEvent;
                        while (loadDialog.pollEvent(dialogEvent)) {
                            if (dialogEvent.type == sf::Event::Closed) {
                                loadDialog.close();
                            }

                            if (dialogEvent.type == sf::Event::MouseButtonPressed) {
                                sf::Vector2f dialogMousePos = loadDialog.mapPixelToCoords(
                                    sf::Mouse::getPosition(loadDialog));

                                for (size_t i = 0; i < saveFiles.size(); i++) {
                                    if (fileTexts[i].getGlobalBounds().contains(dialogMousePos)) {
                                        std::ifstream loadFile(saveFiles[i]);
                                        if (loadFile.is_open()) {
                                            loadFile >> BOARD_ROWS >> BOARD_COLS >> WIDTH >> HEIGHT >> MINES;

                                            int timerStartedInt;
                                            loadFile >> elapsedSeconds >> flagsPlaced >> timerStartedInt;
                                            timerStarted = (timerStartedInt == 1);

                                            int gameOver, gameWon, revealedCount;
                                            loadFile >> gameOver >> gameWon >> revealedCount;

                                            delete board;
                                            board = new Board(BOARD_ROWS, BOARD_COLS, HEIGHT, WIDTH, MINES);
                                            board->setGameOver(gameOver == 1);
                                            board->setGameWon(gameWon == 1);
                                            board->setRevealedCount(revealedCount);
                                            board->setBoardGenerated(true);

                                            for (int a = 0; a < BOARD_ROWS; a++) {
                                                for (int b = 0; b < BOARD_COLS; b++) {
                                                    for (int y = 0; y < HEIGHT; y++) {
                                                        for (int x = 0; x < WIDTH; x++) {
                                                            int value, revealed, flagged;
                                                            loadFile >> value >> revealed >> flagged;
                                                            board->setTile(a, b, x, y, value, revealed, flagged);
                                                        }
                                                    }
                                                }
                                            }
                                            loadFile.close();

                                            if (timerStarted && !board->isGameOver() && !board->isGameWon()) {
                                                gameClock.restart();
                                            }

                                            loadDialog.close();
                                        }
                                    }
                                }

                                if (fileTexts.back().getGlobalBounds().contains(dialogMousePos)) {
                                    loadDialog.close();
                                }
                            }
                        }

                        loadDialog.clear(sf::Color(80, 80, 80));
                        for (const auto& text : fileTexts) {
                            loadDialog.draw(text);
                        }
                        loadDialog.display();
                    }
                }

                activeInput = -1;
                for (int i = 0; i < inputBoxes.size(); i++) {
                    if (inputBoxes[i].getGlobalBounds().contains(mousePos)) {
                        activeInput = i;
                        inputBoxes[i].setOutlineColor(sf::Color::Blue);
                    }
                    else {
                        inputBoxes[i].setOutlineColor(sf::Color(150, 150, 150));
                    }
                }
                if (!needsReset) {
                    float totalWidth = static_cast<float>(BOARD_COLS * (WIDTH * TILE_SIZE) + (BOARD_COLS - 1) * SPACING);
                    float totalHeight = static_cast<float>(BOARD_ROWS * (HEIGHT * TILE_SIZE) + (BOARD_ROWS - 1) * SPACING);
                    float globalOffsetX = (static_cast<float>(window.getSize().x) - totalWidth) / 2.0f;
                    float globalOffsetY = (static_cast<float>(window.getSize().y) - totalHeight) / 2.0f + 100.0f;

                    for (int a = 0; a < BOARD_ROWS; a++) {
                        for (int b = 0; b < BOARD_COLS; b++) {
                            float boardOffsetX = globalOffsetX + b * (WIDTH * TILE_SIZE + SPACING);
                            float boardOffsetY = globalOffsetY + a * (HEIGHT * TILE_SIZE + SPACING);

                            if (mousePos.x >= boardOffsetX &&
                                mousePos.x < boardOffsetX + WIDTH * TILE_SIZE &&
                                mousePos.y >= boardOffsetY &&
                                mousePos.y < boardOffsetY + HEIGHT * TILE_SIZE) {

                                int x = static_cast<int>((mousePos.x - boardOffsetX) / TILE_SIZE);
                                int y = static_cast<int>((mousePos.y - boardOffsetY) / TILE_SIZE);

                                if (!timerStarted && event.mouseButton.button == sf::Mouse::Left) {
                                    timerStarted = true;
                                    gameClock.restart();
                                }

                                bool wasFlagged = board->isFlagged(a, b, x, y);

                                board->handleClick(a, b, x, y, event.mouseButton.button == sf::Mouse::Left);

                                bool nowFlagged = board->isFlagged(a, b, x, y);

                                if (wasFlagged && !nowFlagged) {
                                    flagsPlaced--;
                                }
                                else if (!wasFlagged && nowFlagged) {
                                    flagsPlaced++;
                                }
                            
                            }
                        }
                    }
                }
            }

            if (event.type == sf::Event::TextEntered && activeInput != -1) {
                if (event.text.unicode == '\b') {
                    if (!inputValues[activeInput]->empty())
                        inputValues[activeInput]->pop_back();
                }
                else if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                    if (inputValues[activeInput]->size() < 3) {
                        *inputValues[activeInput] += static_cast<char>(event.text.unicode);
                    }
                }
                inputTexts[activeInput].setString(*inputValues[activeInput]);
            }

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                window.setView(sf::View(visibleArea));
                resetButton.setPosition(static_cast<float>(window.getSize().x) / 2 - TILE_SIZE / 2, 20.0f);
                saveButton.setPosition(static_cast<float>(window.getSize().x) - 150.0f, 20.0f);
                loadButton.setPosition(static_cast<float>(window.getSize().x) - 150.0f, 70.0f);
                saveText.setPosition(saveButton.getPosition().x + 30.0f, saveButton.getPosition().y + 5.0f);
                loadText.setPosition(loadButton.getPosition().x + 30.0f, loadButton.getPosition().y + 5.0f);
            }
        }

        if (timerStarted && !board->isGameOver() && !board->isGameWon()) {
            elapsedSeconds = static_cast<int>(gameClock.getElapsedTime().asSeconds());
        }

        int remainingMines = MINES - flagsPlaced;
        if (remainingMines < 0) remainingMines = 0;
        bombCounterText.setString("Bombs: " + std::to_string(remainingMines));

        int minutes = elapsedSeconds / 60;
        int seconds = elapsedSeconds % 60;
        std::ostringstream timeStream;
        timeStream << "Time: " << std::setw(2) << std::setfill('0') << minutes
            << ":" << std::setw(2) << std::setfill('0') << seconds;
        timerText.setString(timeStream.str());

        if (board->isGameOver()) {
            resetButton.setTextureRect(sf::IntRect(86 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
        }
        else if (board->isGameWon()) {
            resetButton.setTextureRect(sf::IntRect(85 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
        }
        else {
            resetButton.setTextureRect(sf::IntRect(84 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
        }

        if (needsReset) {
            timerStarted = false;
            elapsedSeconds = 0;
            flagsPlaced = 0;

            try {
                BOARD_ROWS = std::stoi(*inputValues[0]);
                BOARD_COLS = std::stoi(*inputValues[1]);
                WIDTH = std::stoi(*inputValues[2]);
                HEIGHT = std::stoi(*inputValues[3]);
                MINES = std::stoi(*inputValues[4]);

                BOARD_ROWS = std::max(1, BOARD_ROWS);
                BOARD_COLS = std::max(1, BOARD_COLS);
                WIDTH = std::max(1, WIDTH);
                HEIGHT = std::max(1, HEIGHT);
                MINES = std::max(1, MINES);

                BOARD_ROWS = std::min(10, BOARD_ROWS);
                BOARD_COLS = std::min(10, BOARD_COLS);
                WIDTH = std::min(20, WIDTH);
                HEIGHT = std::min(20, HEIGHT);

                delete board;
                board = new Board(BOARD_ROWS, BOARD_COLS, HEIGHT, WIDTH, MINES);
            }
            catch (...) {
                *inputValues[0] = "5";
                *inputValues[1] = "5";
                *inputValues[2] = "5";
                *inputValues[3] = "5";
                *inputValues[4] = "40";

                for (int i = 0; i < inputTexts.size(); i++) {
                    inputTexts[i].setString(*inputValues[i]);
                }
            }
            needsReset = false;
        }

        float totalWidth = static_cast<float>(BOARD_COLS * (WIDTH * TILE_SIZE) + (BOARD_COLS - 1) * SPACING);
        float totalHeight = static_cast<float>(BOARD_ROWS * (HEIGHT * TILE_SIZE) + (BOARD_ROWS - 1) * SPACING);

        float globalOffsetX = (static_cast<float>(window.getSize().x) - totalWidth) / 2.0f;
        float globalOffsetY = (static_cast<float>(window.getSize().y) - totalHeight) / 2.0f + 100.0f;

        window.clear(sf::Color(80, 80, 80));

        board->draw(window, tilesTexture, globalOffsetX, globalOffsetY, SPACING);

        window.draw(resetButton);
        window.draw(bombCounterText);
        window.draw(timerText);
        window.draw(saveButton);
        window.draw(loadButton);
        window.draw(saveText);
        window.draw(loadText);

        for (auto& box : inputBoxes) window.draw(box);
        for (auto& label : inputLabels) window.draw(label);
        for (auto& text : inputTexts) window.draw(text);

        if (showInfo) {
            if (infoClock.getElapsedTime().asSeconds() < 2) {
                window.draw(infoText);
            }
            else {
                showInfo = false;
            }
        }

        window.display();
    }

    delete board;
    for (auto ptr : inputValues) delete ptr;

    return 0;
}