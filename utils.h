/*
Utility functions
*/

#ifndef UTILS_H
#define UTILS_H

#include <SFML/Graphics.hpp>
#include <random>


inline double clampf(double x, double min, double max){
    if (x < min) return min;
    if (x > max) return max;
    return x;
}


inline sf::Vector2f clamp2f(sf::Vector2f &v, sf::Vector2f &upperLeft, sf::Vector2f &size){
    return sf::Vector2f(
        clampf(v.x, upperLeft.x, upperLeft.x + size.x),
        clampf(v.y, upperLeft.y - size.y, upperLeft.y)
    );
}


inline bool isInsideSquare(sf::Vector2f &test, sf::Vector2f &upperLeft, sf::Vector2f &size){
    bool insideX = test.x > upperLeft.x && test.x < upperLeft.x + size.x;
    bool insideY = test.y < upperLeft.y && test.x > upperLeft.x - size.x;

    return insideX && insideY;
}

inline float randfloat(float a, float b) {
    // Create a random number generator seeded with a random device
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Define the range [a, b]
    std::uniform_real_distribution<float> dist(a, b);
    
    // Return a random float in the range
    return dist(gen);
}

inline void drawSquareOutline(sf::RenderWindow &window, sf::Vector2f &upperLeft, sf::Vector2f &size, sf::Color color) {
    sf::VertexArray outline(sf::LinesStrip, 5);
    outline[0].position = upperLeft;
    outline[1].position = upperLeft + sf::Vector2f(size.x, 0);
    outline[2].position = upperLeft + sf::Vector2f(size.x, -size.y);
    outline[3].position = upperLeft + sf::Vector2f(0, -size.y);
    outline[4].position = upperLeft;

    for (int i = 0; i < 5; i++) {
        outline[i].color = color;
    }

    window.draw(outline);
}


inline void drawCircle(sf::RenderWindow &window, sf::Vector2f pos, float radius, sf::Color color){
    sf::CircleShape circle(radius);
    circle.setFillColor(color);
    circle.setOrigin(radius, radius);
    circle.setPosition(pos.x, pos.y);
    window.draw(circle);
}


inline void drawBoundary(sf::RenderWindow &window, std::vector<sf::Vector2f> &boundaryVertices2f, sf::Vector2f offset=sf::Vector2f(0, 0)){
    if (boundaryVertices2f.empty()) return;

    sf::VertexArray boundary(sf::LinesStrip, boundaryVertices2f.size());
    for (int i = 0; i < boundaryVertices2f.size(); ++i) {
        boundary[i].position = boundaryVertices2f[i] + offset;
        boundary[i].color = sf::Color::White;
    }
    window.draw(boundary);
}


inline void print(int value) {
    std::cout << value << std::endl;
}

inline void print(double value) {
    std::cout << value << std::endl;
}

inline void print(const std::string& value) {
    std::cout << value << std::endl;
}

inline void print(const char* value) {
    std::cout << value << std::endl;
}

#endif