/*
Scenes
*/

#ifndef SCENE_H
#define SCENE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "wave.h"

struct Scene
{
    std::vector<sf::Vector2f> boundary;
    std::vector<WaveSource> waveSources;

    Scene(std::vector<sf::Vector2f> boundaryVertices2f, std::vector<WaveSource> waveSources)
    : boundary(boundaryVertices2f)
    , waveSources(waveSources)
    {}
};


inline Scene createScene(int choice) {
    std::vector<sf::Vector2f> boundaryVertices2f;
    std::vector<WaveSource> waveSources;
    switch (choice)
    {
    case 0:
        // Classic Chladni Plate
        boundaryVertices2f.push_back(sf::Vector2f(-250, 250));
        boundaryVertices2f.push_back(sf::Vector2f(250, 250));
        boundaryVertices2f.push_back(sf::Vector2f(250, -250));
        boundaryVertices2f.push_back(sf::Vector2f(-250, -250));
        boundaryVertices2f.push_back(sf::Vector2f(-250, 250));
        
        waveSources.push_back(WaveSource(sf::Vector2f(0, 0), 0.2, 0));
        return Scene(boundaryVertices2f, waveSources);
        break;
    case 1:
        // May add more in the future
        break;
    default:
        break;
    }

    return Scene({}, {});
}
#endif
