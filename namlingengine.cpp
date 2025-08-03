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
    std::vector<sf::Sprite> gimbaps; // New: store gimbaps
    float dt = 1.0f / 60.0f;
    float random() {
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(rng);
    }
};

float evalExpr(const std::string& expr, NamlinxContext& ctx);

void runNamlinx(const std::string& script, NamlinxContext& ctx, sf::Texture& thanTex, sf::Texture& gimTex) {
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
            sf::Sprite than(thanTex);
            than.setScale(0.05f, 0.05f);
            than.setPosition(x, y);
            ctx.thanlings.push_back(than);
            continue;
        }

        // New: gim command
        if (line.find("gim") == 0) {
            std::istringstream ss(line);
            std::string cmd;
            float x, y;
            ss >> cmd >> x >> y;
            sf::Sprite gim(gimTex);
            gim.setScale(0.05f, 0.05f);
            gim.setPosition(x, y);
            ctx.gimbaps.push_back(gim);
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
            if (lhs == rhs) runNamlinx(assignment, ctx, thanTex, gimTex);
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
    if (token.find("cos(") == 0 && token.back() == ')') return cos(getValue(token.substr(4, token.size() - 5), ctx));
    if (token.find("sin(") == 0 && token.back() == ')') return sin(getValue(token.substr(4, token.size() - 5), ctx));

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

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "GAME");
    window.setFramerateLimit(60);

    sf::Texture namTex, thanTex, gimTex;
    if (!namTex.loadFromFile("assets/namling.png") || 
        !thanTex.loadFromFile("assets/thanling.png") ||
        !gimTex.loadFromFile("assets/gimbap.png")) {
        std::cerr << "Asset loading failed" << std::endl;
        return 1;
    }

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Font loading failed" << std::endl;
        return 1;
    }

    sf::Sprite namling(namTex);
    namling.setScale(0.05f, 0.05f);

    sf::Text message("", font, 24);
    message.setFillColor(sf::Color::White);

    NamlinxContext ctx;

    std::ifstream scriptFile("assets/script.namx");
    std::string script((std::istreambuf_iterator<char>(scriptFile)), {});

    sf::Vector2f pos(400, 300);

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) if (e.type == sf::Event::Closed) window.close();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) pos.y -= 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) pos.y += 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) pos.x -= 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) pos.x += 2;

        namling.setPosition(pos);

        ctx.vars["box_collided"] = 0;
        ctx.vars["than_collided"] = 0;

        // Box collision & removal
        ctx.boxes.erase(std::remove_if(ctx.boxes.begin(), ctx.boxes.end(), [&](auto& box) {
            if (box.getGlobalBounds().intersects(namling.getGlobalBounds())) {
                ctx.vars["box_collided"] = 1;
                return true;
            }
            return false;
        }), ctx.boxes.end());

        // Thanling collision & removal
        ctx.thanlings.erase(std::remove_if(ctx.thanlings.begin(), ctx.thanlings.end(), [&](auto& than) {
            if (than.getGlobalBounds().intersects(namling.getGlobalBounds())) {
                ctx.vars["than_collided"] = 1;
                return true;
            }
            return false;
        }), ctx.thanlings.end());

        ctx.vars["box_count"] = static_cast<float>(ctx.boxes.size());
        ctx.vars["than_count"] = static_cast<float>(ctx.thanlings.size());

        runNamlinx(script, ctx, thanTex, gimTex);

        if (ctx.str_vars.count("text")) {
            message.setString(ctx.str_vars["text"]);
            float tx = ctx.vars.count("text_x") ? ctx.vars["text_x"] : 10;
            float ty = ctx.vars.count("text_y") ? ctx.vars["text_y"] : 10;
            message.setPosition(tx, ty);
        } else {
            message.setString("");
        }

        window.clear();
        for (auto& b : ctx.boxes) window.draw(b);
        for (auto& t : ctx.thanlings) window.draw(t);
        for (auto& g : ctx.gimbaps) window.draw(g); // Draw gimbaps
        window.draw(namling);
        if (!message.getString().isEmpty()) window.draw(message);
        window.display();
    }
    return 0;
}
