#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

struct Box {
    sf::RectangleShape shape;
};

// Save .nam file (basic XML-style)
void SaveLevel(const std::vector<Box>& boxes, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Could not write file\n";
        return;
    }
    out << "<NamlingLevel>\n";
    for (const auto& box : boxes) {
        auto pos = box.shape.getPosition();
        out << "  <Box x=\"" << pos.x << "\" y=\"" << pos.y << "\"/>\n";
    }
    out << "</NamlingLevel>\n";
    std::cout << "Saved level to " << filename << "\n";
}

// Load .nam file (basic string parsing)
void LoadLevel(std::vector<Box>& boxes, const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return;

    std::string line;
    while (std::getline(in, line)) {
        if (line.find("<Box") != std::string::npos) {
            float x = 0, y = 0;
            size_t xPos = line.find("x=\"");
            size_t yPos = line.find("y=\"");

            if (xPos != std::string::npos && yPos != std::string::npos) {
                x = std::stof(line.substr(xPos + 3, line.find("\"", xPos + 3) - (xPos + 3)));
                y = std::stof(line.substr(yPos + 3, line.find("\"", yPos + 3) - (yPos + 3)));
            }

            Box b;
            b.shape.setSize({50, 50});
            b.shape.setFillColor(sf::Color::Green);
            b.shape.setPosition(x, y);
            boxes.push_back(b);
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Namling Engine");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cout << "Missing assets/arial.ttf\n";
        return -1;
    }

    // Buttons
    sf::Text mode1("Maze Mode", font, 20);
    mode1.setPosition(10, 10);
    sf::RectangleShape btn1({130, 30});
    btn1.setPosition(10, 10);
    btn1.setFillColor(sf::Color(100, 100, 255));

    sf::Text mode2("Collect Mode", font, 20);
    mode2.setPosition(150, 10);
    sf::RectangleShape btn2({150, 30});
    btn2.setPosition(150, 10);
    btn2.setFillColor(sf::Color(100, 255, 100));

    int gameMode = 0;

    // Sprite
    sf::Texture namlingTexture;
    if (!namlingTexture.loadFromFile("assets/namling.png")) {
        std::cout << "Missing assets/namling.png\n";
        return -1;
    }

    sf::Sprite namlingSprite(namlingTexture);
    namlingSprite.setScale(0.05f, 0.05f);
    sf::Vector2f playerPos(400, 300);
    namlingSprite.setPosition(playerPos);

    bool editorMode = false;
    std::vector<Box> boxes;
    LoadLevel(boxes, "namling.nam");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab)
                editorMode = !editorMode;

            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (btn1.getGlobalBounds().contains(mousePos)) gameMode = 1;
                if (btn2.getGlobalBounds().contains(mousePos)) gameMode = 2;

                if (editorMode) {
                    Box b;
                    b.shape.setSize({50, 50});
                    b.shape.setFillColor(sf::Color::Green);
                    b.shape.setPosition(mousePos);
                    boxes.push_back(b);
                }
            }

            else if (editorMode && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                SaveLevel(boxes, "namling.nam");
            }
        }

        // Player movement
        if (!editorMode) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) playerPos.y -= 2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) playerPos.y += 2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) playerPos.x -= 2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) playerPos.x += 2;
            namlingSprite.setPosition(playerPos);
        }

        // Game logic
        if (gameMode == 1) {
            for (auto& b : boxes) {
                if (b.shape.getGlobalBounds().intersects(namlingSprite.getGlobalBounds())) {
                    window.close(); // hit a wall, game over
                }
            }
        } else if (gameMode == 2) {
            for (auto it = boxes.begin(); it != boxes.end(); ) {
                if (it->shape.getGlobalBounds().intersects(namlingSprite.getGlobalBounds()))
                    it = boxes.erase(it);
                else ++it;
            }
        }

        // Draw
        window.clear();
        window.draw(btn1);
        window.draw(mode1);
        window.draw(btn2);
        window.draw(mode2);

        for (auto& b : boxes)
            window.draw(b.shape);

        window.draw(namlingSprite);
        window.display();
    }

    return 0;
}
