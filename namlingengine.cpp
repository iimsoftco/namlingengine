#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <random>
#include <algorithm>

// --- Namlinx Interpreter ---
struct NamlinxContext {
    std::map<std::string, float> vars;
    float dt = 1.0f / 60.0f;
    std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<float> dist{0.f, 1.f};

    float random() { return dist(rng); }
};

// Helper: trim whitespace from string ends
static inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

// Evaluate expressions (supports +, -, *, /, ^ and function calls cos(), sin())
float evalExpr(const std::string& expr, NamlinxContext& ctx);

float getValue(const std::string& token, NamlinxContext& ctx) {
    std::string t = trim(token);

    // Handle functions with parentheses: cos(expr), sin(expr)
    if (t.size() > 4 && t.substr(0, 4) == "cos(" && t.back() == ')') {
        std::string inner = t.substr(4, t.size() - 5);
        return cos(evalExpr(inner, ctx));
    }
    if (t.size() > 4 && t.substr(0, 4) == "sin(" && t.back() == ')') {
        std::string inner = t.substr(4, t.size() - 5);
        return sin(evalExpr(inner, ctx));
    }

    if (t == "dt") return ctx.dt;
    if (t == "random()") return ctx.random();

    // Try to parse as float
    try {
        size_t idx;
        float val = std::stof(t, &idx);
        if (idx == t.size()) return val;
    } catch (...) {}

    // Variable lookup
    if (ctx.vars.count(t)) return ctx.vars[t];

    // Unknown token, default 0
    return 0.f;
}

// Very simple expression parser supporting binary ops and no parentheses except in functions
float evalExpr(const std::string& expr, NamlinxContext& ctx) {
    std::istringstream ss(expr);
    std::string token;
    std::vector<std::string> tokens;

    // Split by space
    while (ss >> token) tokens.push_back(token);

    if (tokens.empty()) return 0;

    // Start with first value
    float result = getValue(tokens[0], ctx);

    // Process operators in pairs (operator value)
    for (size_t i = 1; i + 1 < tokens.size(); i += 2) {
        std::string op = tokens[i];
        float val = getValue(tokens[i + 1], ctx);

        if (op == "+") result += val;
        else if (op == "-") result -= val;
        else if (op == "*") result *= val;
        else if (op == "/") result /= val;
        else if (op == "^") result = pow(result, val);
        else {
            // Unknown operator, ignore
        }
    }
    return result;
}

void runNamlinx(const std::string& script, NamlinxContext& ctx) {
    std::istringstream in(script);
    std::string line;

    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line[0] == '/' || line[0] == '-') continue; // skip comments

        // Assignment (var = expr)
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string var = line.substr(0, eqPos);
            std::string expr = line.substr(eqPos + 1);

            var = trim(var);
            expr = trim(expr);

            float val = evalExpr(expr, ctx);
            ctx.vars[var] = val;
        }
        // if statements: "if condition then statement"
        else if (line.substr(0, 2) == "if") {
            size_t thenPos = line.find("then");
            if (thenPos != std::string::npos) {
                std::string condition = line.substr(2, thenPos - 2);
                std::string statement = line.substr(thenPos + 4);
                condition = trim(condition);
                statement = trim(statement);

                float condVal = evalExpr(condition, ctx);
                if (condVal != 0.f) {
                    // Recursively run statement as a mini-script line
                    runNamlinx(statement, ctx);
                }
            }
        }
    }
}

// --- Game Code (Namling Engine) ---

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Namling Engine v1.3");
    window.setFramerateLimit(60);

    sf::Texture namlingTexture;
    if (!namlingTexture.loadFromFile("assets/namling.png")) {
        std::cerr << "Missing assets/namling.png\n";
        return -1;
    }
    sf::Sprite namling(namlingTexture);
    namling.setScale(0.05f, 0.05f);

    NamlinxContext ctx;

    // Initialize vars used in script
    ctx.vars["angle"] = 0.f;
    ctx.vars["radius"] = 100.f;
    ctx.vars["center_x"] = 400.f;
    ctx.vars["center_y"] = 300.f;
    ctx.vars["x"] = ctx.vars["center_x"] + ctx.vars["radius"];
    ctx.vars["y"] = ctx.vars["center_y"];

    // Load script file
    std::ifstream scriptFile("assets/script.namx");
    std::string script((std::istreambuf_iterator<char>(scriptFile)), std::istreambuf_iterator<char>());
    bool hasScript = !script.empty();

    sf::Vector2f playerPos(ctx.vars["x"], ctx.vars["y"]);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (hasScript) {
            runNamlinx(script, ctx);
            playerPos.x = ctx.vars["x"];
            playerPos.y = ctx.vars["y"];
        } else {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) playerPos.y -= 2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) playerPos.y += 2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) playerPos.x -= 2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) playerPos.x += 2;
        }

        namling.setPosition(playerPos);

        window.clear();
        window.draw(namling);
        window.display();
    }

    return 0;
}
