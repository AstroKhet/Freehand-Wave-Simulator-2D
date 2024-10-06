/*
Sand plate & sand particles
*/

#ifndef SAND_H
#define SAND_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <random>
#include <vector>

#include "wave.h"


class Particle {

public: 
    sf::Vector2f position, velocity;
    sf::Color color;
    float radius;
    // mass ?

    Particle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color, double radius)
        : position(position)
        , velocity(velocity)
        , color(color)
        , radius(radius)
    {}

    void update(double dt, sf::Vector2f acceleration){
        velocity += sf::Vector2f(acceleration.x * dt, acceleration.y * dt);
        
        // Damping
        // velocity = sf::Vector2f(velocity.x * 0.95f, velocity.y * 0.95f);
        position += sf::Vector2f(velocity.x * dt, velocity.y * dt);
        

    }
};

class Sand {

public:
    float radius;
    std::vector<Particle> particles;
    Wave *WavePlate;
    sf::Vector2f displayPosition, platePos, offset;

    Sand(Wave &Plate, sf::Vector2f displayPosition, sf::Vector2f platePos)
        : WavePlate(&Plate) 
        , displayPosition(displayPosition)
        , platePos(platePos)
    {

    };

    void addParticle(Particle particle) {
        particles.push_back(particle);
    }

    void update(double dt) {
        if (!WavePlate->simulating) return;

        // Remove particles not on WavePlate
        particles.erase(std::remove_if(particles.begin(), particles.end(), [this](Particle particle) -> bool {
            int x = static_cast<int>(std::round(particle.position.x));
            int y = static_cast<int>(std::round(particle.position.y));
            sf::Vector2f test = sf::Vector2f(x, y) + WavePlate->offset;
            return !WavePlate->isInsideBoundary(test);
        }), particles.end());

        // Update particle positions based on approximated acceleration
        dt = std::min(dt, 1.0 / WavePlate->alpha);
        double dt2 = dt * dt;

        for (auto &particle : particles){
            int x = static_cast<int>(std::round(particle.position.x));
            int y = static_cast<int>(std::round(particle.position.y));

            double mid   = WavePlate->u_1[y][x];
            double up    = WavePlate->isOnGrid(x, y+1) ? WavePlate->u_1[y+1][x] : mid;
            double right = WavePlate->isOnGrid(x+1, y) ? WavePlate->u_1[y][x+1] : mid;
            double down  = WavePlate->isOnGrid(x, y-1) ? WavePlate->u_1[y-1][x] : mid;
            double left  = WavePlate->isOnGrid(x-1, y) ? WavePlate->u_1[y][x-1] : mid;

            double d2x_dt2 = (right - 2 * mid + left) / dt2;
            double d2y_dt2 = (up    - 2 * mid + down) / dt2;
            
            sf::Vector2f estimated_accel(d2x_dt2 * 0.2, d2y_dt2 * 0.2);

            // double u = std::abs(WavePlate->u_1[y][x]);
            // double r = 20 * u * u * u;
            // sf::Vector2f estimated_accel(randfloat(-r, r), randfloat(-r, r));

            particle.update(dt, estimated_accel);
        }
    }

    void reset(){
        particles.clear();
    }

    void begin(){
        offset = WavePlate->offset + displayPosition - platePos;
    }


    void draw(sf::RenderWindow &window) {
        if (!WavePlate->simulating) return;

        for (auto &particle : particles){
            drawCircle(window, particle.position + offset, particle.radius, particle.color);
        }
    }
};

#endif