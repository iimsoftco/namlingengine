#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <random>
#include <algorithm>
#include <set>

// --- Namlinx Interpreter ---
struct NamlinxContext {
    std::map<std::string, float> vars;
    std::map<std::string, std::string> str_vars;
    std::vector<sf::RectangleShape> boxes;
    std::vector<sf::Sprite> thanlings;
    float dt = 1.0f / 60.0f;
    float random() {
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(rng);
    }
};

float evalExpr(const std::string& expr, NamlinxContext& ctx);

void runNamlinx(const std::string& script, NamlinxContext& ctx, sf::Texture& thanTexture) {
    std::istringstream in(script);
    std::string line;
    while (std::getline(in, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '/' || line[0] == '-') continue;

        if (line.find('=') != std::string::npos && line.find('"') != std::string::npos) {
            size_t eq = line.find('=');
            std::string var = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            var.erase(remove_if(var.begin(), var.end(), ::isspace), var.end());
            value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end());
            if (value.front() == '"' && value.back() == '"') {
                ctx.str_vars[var] = value.substr(1, value.size() - 2);
            }
            continue;
        }

        if (line.find("box") == 0) {
            std::istringstream ss(line);
            std::string cmd;
            float x, y, size;
            std::string color;
            ss >> cmd >> x >> y >> size >> color;
            sf::RectangleShape box(sf::Vector2f(size, size));
            box.setPosition(x, y);
            if (color == "green") box.setFillColor(sf::Color::Green);
            else if (color == "red") box.setFillColor(sf::Color::Red);
            else if (color == "blue") box.setFillColor(sf::Color::Blue);
            else box.setFillColor(sf::Color::White);
            ctx.boxes.push_back(box);
            continue;
        }

        if (line.find("than") == 0) {
            std::istringstream ss(line);
            std::string cmd;
            float x, y;
            ss >> cmd >> x >> y;
            sf::Sprite s(thanTexture);
            s.setScale(0.05f, 0.05f);
            s.setPosition(x, y);
            ctx.thanlings.push_back(s);
            continue;
        }

        if (line.find("==") != std::string::npos && line.find("then") != std::string::npos) {
            size_t varEnd = line.find("==");
            size_t thenPos = line.find("then");
            std::string var = line.substr(0, varEnd);
            std::string condition = line.substr(varEnd + 2, thenPos - varEnd - 2);
            std::string assignment = line.substr(thenPos + 4);

            float lhs = evalExpr(var, ctx);
            float rhs = evalExpr(condition, ctx);
            if (lhs == rhs) runNamlinx(assignment, ctx, thanTexture);
        } else if (line.find("=") != std::string::npos) {
            size_t eq = line.find("=");
            std::string var = line.substr(0, eq);
            std::string expr = line.substr(eq + 1);
            var.erase(remove_if(var.begin(), var.end(), ::isspace), var.end());
            float val = evalExpr(expr, ctx);
            ctx.vars[var] = val;
        }
    }
}

float getValue(const std::string& token, NamlinxContext& ctx) {
    if (token == "dt") return ctx.dt;
    if (token == "random()") return ctx.random();

    if (token.find("cos(") == 0 && token.back() == ')') {
        std::string inner = token.substr(4, token.size() - 5);
        return cos(getValue(inner, ctx));
    }
    if (token.find("sin(") == 0 && token.back() == ')') {
        std::string inner = token.substr(4, token.size() - 5);
        return sin(getValue(inner, ctx));
    }

    try {
        return std::stof(token);
    } catch (...) {
        if (ctx.vars.count(token)) return ctx.vars[token];
    }
    return 0;
}

float evalExpr(const std::string& expr, NamlinxContext& ctx) {
    std::istringstream ss(expr);
    std::string token;
    std::vector<std::string> tokens;
    while (ss >> token) tokens.push_back(token);
    if (tokens.size() == 1) return getValue(tokens[0], ctx);
    float result = getValue(tokens[0], ctx);
    for (size_t i = 1; i + 1 < tokens.size(); i += 2) {
        std::string op = tokens[i];
        float val = getValue(tokens[i + 1], ctx);
        if (op == "+") result += val;
        else if (op == "-") result -= val;
        else if (op == "*") result *= val;
        else if (op == "/") result /= val;
        else if (op == "^") result = pow(result, val);
    }
    return result;
}

// --- Main Game Loop ---
int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "GAME");
    window.setFramerateLimit(60);

    sf::Texture namlingTexture;
    if (!namlingTexture.loadFromFile("assets/namling.png")) {
        std::cerr << "Missing assets/namling.png\n";
        return 1;
    }

    sf::Texture thanTexture;
    if (!thanTexture.loadFromFile("assets/thanling.png")) {
        std::cerr << "Missing assets/thanling.png\n";
        return 1;
    }

    sf::Sprite namling(namlingTexture);
    namling.setScale(0.05f, 0.05f);
    sf::Vector2f playerPos(400, 300);

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Missing assets/arial.ttf\n";
        return 1;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    NamlinxContext ctx;
    std::ifstream scriptFile("assets/script.namx");
    std::string script((std::istreambuf_iterator<char>(scriptFile)), std::istreambuf_iterator<char>());
    bool hasScript = !script.empty();

    std::set<size_t> touchedThanlings;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
        }

        // Movement (always available)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) playerPos.y -= 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) playerPos.y += 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) playerPos.x -= 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) playerPos.x += 2;
        namling.setPosition(playerPos);

        // Run script
        if (hasScript) {
            runNamlinx(script, ctx, thanTexture);
            if (ctx.vars.count("x")) playerPos.x = ctx.vars["x"];
            if (ctx.vars.count("y")) playerPos.y = ctx.vars["y"];
            namling.setPosition(playerPos);
        }

        // Handle thanling collisions
        touchedThanlings.clear();
        for (size_t i = 0; i < ctx.thanlings.size(); ++i) {
            if (ctx.thanlings[i].getGlobalBounds().intersects(namling.getGlobalBounds())) {
                touchedThanlings.insert(i);
            }
        }
        ctx.vars["than_touched_count"] = static_cast<float>(touchedThanlings.size());
        ctx.vars["than_total"] = static_cast<float>(ctx.thanlings.size());

        // Handle box collisions
        std::vector<sf::RectangleShape> newBoxes;
        for (auto& box : ctx.boxes) {
            if (!box.getGlobalBounds().intersects(namling.getGlobalBounds())) {
                newBoxes.push_back(box);
            }
        }
        ctx.boxes = std::move(newBoxes);
        ctx.vars["box_count"] = static_cast<float>(ctx.boxes.size());

        // Text rendering
        if (ctx.str_vars.count("text")) {
            text.setString(ctx.str_vars["text"]);
            float tx = ctx.vars.count("text_x") ? ctx.vars["text_x"] : 10;
            float ty = ctx.vars.count("text_y") ? ctx.vars["text_y"] : 10;
            text.setPosition(tx, ty);
        } else {
            text.setString("");
        }

        window.clear();
        for (auto& b : ctx.boxes) window.draw(b);
        for (auto& t : ctx.thanlings) window.draw(t);
        window.draw(namling);
        if (!text.getString().isEmpty()) window.draw(text);
        window.display();
    }

    return 0;
}
