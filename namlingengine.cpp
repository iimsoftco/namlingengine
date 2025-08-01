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
    float random() {
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(rng);
    }
};

float evalExpr(const std::string& expr, NamlinxContext& ctx);

void runNamlinx(const std::string& script, NamlinxContext& ctx) {
    std::istringstream in(script);
    std::string line;
    while (std::getline(in, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '/' || line[0] == '-') continue;

        if (line.find("==") != std::string::npos && line.find("then") != std::string::npos) {
            size_t varEnd = line.find("==");
            size_t thenPos = line.find("then");
            std::string var = line.substr(3, varEnd - 3);
            std::string condition = line.substr(varEnd + 2, thenPos - varEnd - 2);
            std::string assignment = line.substr(thenPos + 4);

            float lhs = evalExpr(var, ctx);
            float rhs = evalExpr(condition, ctx);
            if (lhs == rhs) runNamlinx(assignment, ctx);
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

    // Functions
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

// --- Game Code (Namling Engine) ---

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Namling Engine v1.4");
    window.setFramerateLimit(60);

    sf::Texture namlingTexture;
    namlingTexture.loadFromFile("assets/namling.png");
    sf::Sprite namling(namlingTexture);
    namling.setScale(0.05f, 0.05f);

    NamlinxContext ctx;

    std::ifstream scriptFile("assets/script.namx");
    std::string script;
    if (scriptFile) {
        script.assign((std::istreambuf_iterator<char>(scriptFile)), std::istreambuf_iterator<char>());
    }
    bool hasScript = !script.empty();

    sf::Vector2f playerPos(400, 300);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (hasScript) {
            runNamlinx(script, ctx);
            if (ctx.vars.count("x")) playerPos.x = ctx.vars["x"];
            if (ctx.vars.count("y")) playerPos.y = ctx.vars["y"];
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
