#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <cmath>

// Box structure for obstacles/collectibles
struct Box {
    sf::RectangleShape shape;
};

// Simple Namlinx interpreter supporting:
// - variables (x,y,speed)
// - if isKeyDown("KEY") then var = var +/- speed * dt
// Only supports lines like:
//   speed = 150
//   if isKeyDown("W") then y = y - speed * dt
//   if isKeyDown("S") then y = y + speed * dt
// etc.
class NamlinxInterpreter {
public:
    float x = 0.f, y = 0.f;
    float speed = 150.f;

    std::vector<std::string> scriptLines;

    bool loaded = false;

    // Load .namx script from file
    void LoadScript(const std::string& path) {
        std::ifstream in(path);
        if (!in.is_open()) {
            std::cerr << "[Namlinx] Could not open " << path << "\n";
            loaded = false;
            return;
        }
        scriptLines.clear();
        std::string line;
        while (std::getline(in, line)) {
            // trim whitespace (simple)
            size_t first = line.find_first_not_of(" \t\r\n");
            size_t last = line.find_last_not_of(" \t\r\n");
            if (first == std::string::npos) continue;
            scriptLines.push_back(line.substr(first, last - first + 1));
        }
        loaded = true;
        std::cout << "[Namlinx] Loaded script with " << scriptLines.size() << " lines\n";
    }

    // Utility: check if a key is down from map (keys passed from SFML)
    bool isKeyDown(const std::string& keyName, const std::unordered_map<std::string, bool>& keyMap) {
        auto it = keyMap.find(keyName);
        return it != keyMap.end() && it->second;
    }

    // Evaluate simple expression of form var = var +/- speed * dt
    void ExecuteLine(const std::string& line, float dt, const std::unordered_map<std::string, bool>& keyMap) {
        // speed = number
        if (line.find("speed =") == 0) {
            std::string valStr = line.substr(7);
            speed = std::stof(valStr);
            return;
        }

        // if isKeyDown("KEY") then var = var +/- speed * dt
        // Parse with minimal string ops
        if (line.find("if isKeyDown(") == 0) {
            size_t keyStart = line.find('"');
            size_t keyEnd = line.find('"', keyStart + 1);
            if (keyStart == std::string::npos || keyEnd == std::string::npos) return;

            std::string keyName = line.substr(keyStart + 1, keyEnd - keyStart - 1);

            if (!isKeyDown(keyName, keyMap)) return; // condition false, skip

            // find 'then'
            size_t thenPos = line.find("then", keyEnd);
            if (thenPos == std::string::npos) return;

            std::string expr = line.substr(thenPos + 4);
            // Expect: var = var +/- speed * dt
            // e.g. y = y - speed * dt

            // parse var name on left
            size_t eqPos = expr.find('=');
            if (eqPos == std::string::npos) return;

            std::string leftVar = Trim(expr.substr(0, eqPos));
            std::string rightExpr = Trim(expr.substr(eqPos + 1));

            // rightExpr example: y - speed * dt
            // parse components naively:

            // var op speed * dt
            char op = '+';
            size_t opPos = rightExpr.find('+');
            if (opPos == std::string::npos) {
                opPos = rightExpr.find('-');
                if (opPos != std::string::npos) op = '-';
            }

            if (opPos == std::string::npos) return;

            std::string varOnRight = Trim(rightExpr.substr(0, opPos));
            std::string rest = Trim(rightExpr.substr(opPos + 1));

            // rest should be "speed * dt"
            if (rest != "speed * dt") return;

            // Update the correct variable
            float delta = speed * dt;

            if (leftVar == "x" && varOnRight == "x") {
                if (op == '+') x += delta;
                else x -= delta;
            }
            else if (leftVar == "y" && varOnRight == "y") {
                if (op == '+') y += delta;
                else y -= delta;
            }
        }
    }

    // Trim helper
    static std::string Trim(const std::string& s) {
        size_t first = s.find_first_not_of(" \t");
        size_t last = s.find_last_not_of(" \t");
        if (first == std::string::npos) return "";
        return s.substr(first, (last - first + 1));
    }

    // Update interpreter per frame
    // dt = delta time seconds
    // keyMap = current keys pressed
    // Updates x,y positions based on script
    void Update(float dt, std::unordered_map<std::string, bool>& keyMap) {
        if (!loaded) return;

        for (const auto& line : scriptLines) {
            ExecuteLine(line, dt, keyMap);
        }
    }
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
    sf::RenderWindow window(sf::VideoMode(800, 600), "Namling Engine v1.2");
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

    bool editorMode = false;
    std::vector<Box> boxes;
    LoadLevel(boxes, "namling.nam");

    // Interpreter for custom behavior
    NamlinxInterpreter interpreter;

    // Try load .namx script
    interpreter.LoadScript("assets/behavior.namx");

    // Player position init
    sf::Vector2f playerPos(400, 300);
    interpreter.x = playerPos.x;
    interpreter.y = playerPos.y;

    // Save confirmation text
    sf::Text saveMsg("Game saved as .nam", font, 18);
    saveMsg.setFillColor(sf::Color::White);
    saveMsg.setPosition(10, 550);
    bool showSaveMsg = false;
    sf::Clock saveMsgClock;

    sf::Clock deltaClock;

    while (window.isOpen()) {
        float dt = deltaClock.restart().asSeconds();

        // Gather key states for Namlinx
        std::unordered_map<std::string, bool> keyMap;
        keyMap["W"] = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
        keyMap["A"] = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        keyMap["S"] = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        keyMap["D"] = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab)
                editorMode = !editorMode;

            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (btn1.getGlobalBounds().contains(mousePos)) {
                    gameMode = 1;
                    std::cout << "Selected Maze Mode\n";
                }
                if (btn2.getGlobalBounds().contains(mousePos)) {
                    gameMode = 2;
                    std::cout << "Selected Collect Mode\n";
                }

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
                showSaveMsg = true;
                saveMsgClock.restart();
            }
        }

        // Update player position using Namlinx or fallback keyboard
        if (!editorMode) {
            if (interpreter.loaded) {
                interpreter.Update(dt, keyMap);
                playerPos.x = interpreter.x;
                playerPos.y = interpreter.y;
            }
            else {
                if (keyMap["W"]) playerPos.y -= 150.f * dt;
                if (keyMap["S"]) playerPos.y += 150.f * dt;
                if (keyMap["A"]) playerPos.x -= 150.f * dt;
                if (keyMap["D"]) playerPos.x += 150.f * dt;
            }
            namlingSprite.setPosition(playerPos);
        }

        // Game logic
        if (gameMode == 1) { // Maze Mode: touching box = game over (close window)
            for (auto& b : boxes) {
                if (b.shape.getGlobalBounds().intersects(namlingSprite.getGlobalBounds())) {
                    std::cout << "Hit obstacle! Game Over.\n";
                    window.close();
                }
            }
        }
        else if (gameMode == 2) { // Collect Mode: touching box removes it
            for (auto it = boxes.begin(); it != boxes.end();) {
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

        if (showSaveMsg) {
            window.draw(saveMsg);
            if (saveMsgClock.getElapsedTime().asSeconds() > 3.f)
                showSaveMsg = false;
        }

        window.display();
    }

    return 0;
}
