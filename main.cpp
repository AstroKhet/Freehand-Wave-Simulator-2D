#include <SFML/Graphics.hpp>

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

#include "wave.h"
#include "sand.h"
#include "scene.h"
#include "utils.h"


int main()
{
    printf("Start\n");

    // WINDOW CONFIGURATIONS
    const int W_WIDTH = 1100;
    const int W_HEIGHT = 1100;

    sf::RenderWindow window(sf::VideoMode(W_WIDTH, W_HEIGHT), "WavPro2D");

    // VIRTUAL SIMULTATION CONFIGURATION
    const int V_WIDTH = W_WIDTH/2;
    const int V_HEIGHT = W_HEIGHT/2;
    const float distBetweenVertices = 5;
    sf::View view(sf::Vector2f(0, 0), sf::Vector2f(V_WIDTH, -V_HEIGHT));
    window.setView(view);

    // PLATE INPUT VIEWBOX
    bool viewPlate = true;
    double c = 10, waveFrequency = 0.2;
    sf::Vector2f plateInputPos(-250, 250);
    sf::Vector2f plateInputSize(500, 500);
    Wave WavePlate(c, plateInputPos, plateInputSize);
    
    // SAND VIEWBOX
    double r = 0.2;
    // sf::Vector2f sandViewPos(30, V_HEIGHT/2 - 50);
    sf::Vector2f sandViewPos = plateInputPos;
    sf::Vector2f sandViewSize(500, 500);
    Sand SandPlate(WavePlate, sandViewPos, plateInputPos);

    // EVENT LOOP VARIABLES
    double dt, elapsed_t = 0.0;
    double lastPlacedWaveSource = 0.0;
    std::chrono::steady_clock::time_point last_t = std::chrono::steady_clock::now();

    bool leftMouseDown = false, rightMouseDown = false;
    sf::Vector2f mousePosition;
    
    // MAIN EVENT LOOP
    while (window.isOpen())
    {
        // TIME UTILITIES
        double dt = std::chrono::duration<double>(std::chrono::steady_clock::now() - last_t).count();
        last_t = std::chrono::steady_clock::now();
        elapsed_t += dt;

        // WINDOW EVENTS
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();

            mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) viewPlate = !viewPlate;

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                leftMouseDown = true;
                if (!WavePlate.boundaryVertices2f.empty()) {
                    WavePlate.boundaryVertices2f.clear();
                }
                WavePlate.reset();
                SandPlate.reset();
                WavePlate.boundaryVertices2f.push_back(clamp2f(mousePosition, plateInputPos, plateInputSize));
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            {
                leftMouseDown = false;
                if (WavePlate.boundaryVertices2f.size() >= 2){
                    WavePlate.boundaryVertices2f.push_back(WavePlate.boundaryVertices2f.front());
                    WavePlate.begin(dt);
                    SandPlate.begin();
                }
            }
            if (leftMouseDown)
            {
                sf::Vector2f sep = WavePlate.boundaryVertices2f.back() - mousePosition;
                if (sep.x * sep.x + sep.y * sep.y > distBetweenVertices * distBetweenVertices)
                {
                    WavePlate.boundaryVertices2f.push_back(clamp2f(mousePosition, plateInputPos, plateInputSize));
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                if (isInsideSquare(mousePosition, plateInputPos, plateInputSize) && elapsed_t - lastPlacedWaveSource > 0.2)
                {
                    WavePlate.addWaveSource(WaveSource(mousePosition, waveFrequency, elapsed_t));
                    lastPlacedWaveSource = elapsed_t;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                for (int i = 0; i < 10; i++){
                    sf::Vector2f sandPosition = mousePosition + sf::Vector2f(randfloat(-6, 6), randfloat(-6, 6));
                    if (isInsideSquare(sandPosition, sandViewPos, sandViewSize))
                    {
                        SandPlate.addParticle(Particle(
                            sandPosition - sandViewPos - WavePlate.offset + plateInputPos,
                            sf::Vector2f(0, 0),
                            sf::Color::Yellow,
                            0.5f
                        ));
                    }
                }
            }

             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) {
                Scene chladni = createScene(0);
                WavePlate.boundaryVertices2f = chladni.boundary;
                WavePlate.wavePoints = chladni.waveSources;
                WavePlate.begin(dt);
                SandPlate.begin();
             }
        }

        window.clear();
        
        if (viewPlate) WavePlate.draw(window);
        WavePlate.update(dt, elapsed_t);


        SandPlate.update(dt);
        SandPlate.draw(window);

        drawBoundary(window, WavePlate.boundaryVertices2f);
        drawBoundary(window, WavePlate.boundaryVertices2f, sandViewPos - plateInputPos);
        
        drawSquareOutline(window, plateInputPos, plateInputSize, sf::Color::White);
        drawSquareOutline(window, sandViewPos  , sandViewSize  , sf::Color::White);

        drawCircle(window, mousePosition, 2.0f, sf::Color::Red);
        
        window.display();
    }

    return 0;
}