#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <random>
#include <algorithm>

// -------- Namlinx Interpreter --------
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
            std::string var = line.substr(0, varEnd);
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
    if (ctx.vars.count(token)) return ctx.vars[token];

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
    } catch (...) {}
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

// -------- Main Game --------

struct Box {
    sf::RectangleShape shape;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "GAME");
    window.setFramerateLimit(60);

    sf::Texture namlingTexture, gimbapTexture, thanlingTexture;
    if (!namlingTexture.loadFromFile("assets/namling.png")) {
        std::cerr << "Failed to load namling.png\n";
        return 1;
    }
    if (!gimbapTexture.loadFromFile("assets/gimbap.png")) {
        std::cerr << "Failed to load gimbap.png\n";
        return 1;
    }
    if (!thanlingTexture.loadFromFile("assets/thanling.png")) {
        std::cerr << "Failed to load thanling.png\n";
        return 1;
    }

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
    std::vector<Box> boxes;
    std::vector<sf::Sprite> gimbaps;
    std::vector<sf::Sprite> thanlings;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update variables for script
        ctx.vars["mouse_x"] = static_cast<float>(sf::Mouse::getPosition(window).x);
        ctx.vars["mouse_y"] = static_cast<float>(sf::Mouse::getPosition(window).y);

        if (hasScript) {
            runNamlinx(script, ctx);

            // Update player position
            if (ctx.vars.count("x")) playerPos.x = ctx.vars["x"];
            if (ctx.vars.count("y")) playerPos.y = ctx.vars["y"];

            // Create boxes from script vars
            boxes.clear();
            for (int i = 0; i < 100; ++i) {
                std::string bx = "box" + std::to_string(i) + "_x";
                std::string by = "box" + std::to_string(i) + "_y";
                std::string bw = "box" + std::to_string(i) + "_w";
                std::string bh = "box" + std::to_string(i) + "_h";
                std::string bc = "box" + std::to_string(i) + "_color";

                if (ctx.vars.count(bx) && ctx.vars.count(by)) {
                    Box b;
                    float w = ctx.vars.count(bw) ? ctx.vars[bw] : 50;
                    float h = ctx.vars.count(bh) ? ctx.vars[bh] : 50;
                    int colorCode = ctx.vars.count(bc) ? static_cast<int>(ctx.vars[bc]) : 0;

                    b.shape.setSize({w, h});
                    switch (colorCode) {
                        case 1: b.shape.setFillColor(sf::Color::Green); break;
                        case 2: b.shape.setFillColor(sf::Color::Blue); break;
                        case 3: b.shape.setFillColor(sf::Color::Red); break;
                        default: b.shape.setFillColor(sf::Color::White); break;
                    }

                    b.shape.setPosition(ctx.vars[bx], ctx.vars[by]);
                    boxes.push_back(b);
                }
            }

            // Create gimbaps from script vars
            gimbaps.clear();
            for (int i = 0; i < 100; ++i) {
                std::string gx = "gim" + std::to_string(i) + "_x";
                std::string gy = "gim" + std::to_string(i) + "_y";

                if (ctx.vars.count(gx) && ctx.vars.count(gy)) {
                    sf::Sprite g(gimbapTexture);
                    g.setScale(0.05f, 0.05f);
                    g.setPosition(ctx.vars[gx], ctx.vars[gy]);
                    gimbaps.push_back(g);
                }
            }

            // Create thanlings from script vars
            thanlings.clear();
            for (int i = 0; i < 100; ++i) {
                std::string tx = "than" + std::to_string(i) + "_x";
                std::string ty = "than" + std::to_string(i) + "_y";

                if (ctx.vars.count(tx) && ctx.vars.count(ty)) {
                    sf::Sprite t(thanlingTexture);
                    t.setScale(0.05f, 0.05f);
                    t.setPosition(ctx.vars[tx], ctx.vars[ty]);
                    thanlings.push_back(t);
                }
            }
        
            // Manual WASD movement fallback
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) playerPos.y -= 2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) playerPos.y += 2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) playerPos.x -= 2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) playerPos.x += 2;
        }

        namling.setPosition(playerPos);

        // Check collision and remove boxes on contact
        for (auto it = boxes.begin(); it != boxes.end();) {
            if (namling.getGlobalBounds().intersects(it->shape.getGlobalBounds())) {
                ctx.vars["box_collided"] = 1;
                it = boxes.erase(it);
            } else {
                ++it;
            }
        }

        // Check collision and remove gimbaps on contact
        for (auto it = gimbaps.begin(); it != gimbaps.end();) {
            if (namling.getGlobalBounds().intersects(it->getGlobalBounds())) {
                ctx.vars["gim_collided"] = 1;
                it = gimbaps.erase(it);
            } else {
                ++it;
            }
        }

        // Check collision and remove thanlings on contact
        for (auto it = thanlings.begin(); it != thanlings.end();) {
            if (namling.getGlobalBounds().intersects(it->getGlobalBounds())) {
                ctx.vars["than_collided"] = 1;
                it = thanlings.erase(it);
            } else {
                ++it;
            }
        }

        ctx.vars["box_count"] = static_cast<float>(boxes.size());
        ctx.vars["gim_count"] = static_cast<float>(gimbaps.size());
        ctx.vars["than_count"] = static_cast<float>(thanlings.size());

        window.clear();
        for (auto& b : boxes) window.draw(b.shape);
        for (auto& g : gimbaps) window.draw(g);
        for (auto& t : thanlings) window.draw(t);
        window.draw(namling);
        window.display();
    }

    return 0;
}
