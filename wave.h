/*
Wave plates & wave sources
*/

#ifndef WAVE_H
#define WAVE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

#include "utils.h"

class WaveSource {

public:
    sf::Vector2f point;
    double freq, t0;  // MAYBE CHANGE DATATYPE TO DOUBLE?
    double freq2pi;  // freq * 2pi

    WaveSource(sf::Vector2f point2f, double freq, double t0) 
        : freq(freq)
        , t0(t0) 
    {
        point = sf::Vector2f(static_cast<int>(point2f.x), static_cast<int>(point2f.y));
        freq2pi = freq * 2 * M_PI;;
    }

    double at(double t){
        return sin(freq2pi * (t - t0));
    }
};


class Wave
{

public:
    // Simulation variables
    std::vector<std::vector<double>> u_0, u_1, u_2;
    std::vector<WaveSource> wavePoints;
    double dt0, alpha;

    // Program variables
    std::vector<sf::Vector2f> boundaryVertices2f;
    std::vector<std::vector<bool>> platePixels;
    sf::Vector2f offset;  // Offset for upperleft of bounding rectangle of boundary vertices
    bool boundaryIsDefined = false;
    bool simulating = false;
    int height, width;

    sf::Vector2f displayPosition;
    sf::Vector2f displaySize;

    Wave(double alpha, sf::Vector2f displayPosition, sf::Vector2f displaySize) 
        : alpha(alpha)
        , displayPosition(displayPosition)
        , displaySize(displaySize) 
    {};

    void setWaveSource(WaveSource waveSource){
        if (isInsideBoundary(waveSource.point)) wavePoints = {waveSource};
    }

    void addWaveSource(WaveSource waveSource){
        if (isInsideBoundary(waveSource.point)) wavePoints.push_back(waveSource);
    }


    void begin(double dt){
        // Create Plate Pixels
        int minX = displayPosition.x + displaySize.x;
        int maxX = displayPosition.x;
        int minY = displayPosition.y;
        int maxY = displayPosition.y - displaySize.y;

        for (auto &v : boundaryVertices2f) {
            minX = (v.x < minX) ? v.x : minX;
            maxX = (v.x > maxX) ? v.x : maxX;
            minY = (v.y < minY) ? v.y : minY;
            maxY = (v.y > maxY) ? v.y : maxY;
        }

        offset = sf::Vector2f(minX, minY);

        height = maxY - minY;
        width  = maxX - minX;
         
        platePixels.resize(height, std::vector<bool>(width, false));
        u_0.resize(height, std::vector<double>(width, 0));
        u_1.resize(height, std::vector<double>(width, 0));
        u_2.resize(height, std::vector<double>(width, 0));


        for (int y = 0; y < height; y++){   // OPTIMIZE THIS
            for (int x = 0; x < width; x++){  // OPTIMIZE THIS
                sf::Vector2f test = sf::Vector2f(x, y) + offset;
                platePixels[y][x] = isInsideBoundary(test);  // OPTIMIZE THIS
            }
        } 

        boundaryIsDefined = true;
        dt0 = std::min(dt, 1.0f / alpha);
        simulating = true;

        updateWavePoints(u_0, 0);
        updateWavePoints(u_1, dt0);
    }

    
    void update(double dt1, double t){
        if (!simulating) return;

        dt1 = std::min(dt1, 1.0 / alpha);
        updateWavePoints(u_2, t);

        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){
                if (isWavePoint(x, y)) continue;
                if (!platePixels[y][x]) continue;

                double mid   = u_1[y][x];
                double up    = isOnGrid(x, y+1) ? u_1[y+1][x] : mid;
                double right = isOnGrid(x+1, y) ? u_1[y][x+1] : mid;
                double down  = isOnGrid(x, y-1) ? u_1[y-1][x] : mid;
                double left  = isOnGrid(x-1, y) ? u_1[y][x-1] : mid;

                double stencil = -4 * mid + up + right + down + left;
                double du_dt0 = (u_1[y][x] - u_0[y][x]) / dt0;
                u_2[y][x] = dt1 * (alpha * stencil + du_dt0) + u_1[y][x];

                // u_2[y][x] = 2 * u_1[y][x] - u_0[y][x] + alpha * alpha * 0.01 * 0.01 * stencil;
            }
        }

        std::swap(u_0, u_1);
        std::swap(u_1, u_2);  // After update(), refer to u_1 for latest numerical solution
        std::swap(dt0, dt1);
    }


    void reset(){
        platePixels.clear();
        u_0.clear();
        u_1.clear();
        u_2.clear();

        boundaryIsDefined = false;
        simulating = false;
        wavePoints = {};
    }


    void draw(sf::RenderWindow &window){
        if (!simulating) return;
        if (width == 0 || height == 0) return;

        sf::Image platePixelsImage;
        platePixelsImage.create(width, height, sf::Color::Transparent);

        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){
                if (platePixels[y][x]){  // OPTIMIZE THIS
                    platePixelsImage.setPixel(x, y, toGreyscale(u_1[y][x]));
                }
            }
        }

        sf::Texture texture; texture.loadFromImage(platePixelsImage);
        sf::Sprite sprite(texture); sprite.setPosition(offset);
        window.draw(sprite);
    }


    bool isInsideBoundary(sf::Vector2f &testPoint)
    {
        // tests if a point is literally inside the boundary (i.e. visually inside, no offsets)
        bool inside = false;
        for (int i = 0; i < boundaryVertices2f.size() - 1; i++)
        {
            sf::Vector2f pointA = boundaryVertices2f[i];
            sf::Vector2f pointB = boundaryVertices2f[i+1];

            if (((pointA.y > testPoint.y) != (pointB.y > testPoint.y)) && 
                 (testPoint.x < (pointB.x - pointA.x) * (testPoint.y - pointA.y) / (pointB.y - pointA.y) + pointA.x))
            {
                inside = !inside;
            }
        }
        return inside;
    }

    void updateWavePoints(std::vector<std::vector<double>> &u, double t){
        for (auto &source : wavePoints) {
            u[source.point.y - offset.y][source.point.x - offset.x] = source.at(t);
        }
    }

    bool isWavePoint(int x, int y){
        for (auto &source : wavePoints){
            sf::Vector2f test = sf::Vector2f(x, y) + offset;
            if (test.x == source.point.x && test.y == source.point.y) return true;
        }
        return false;
    }


    bool isOnGrid(int x, int y){
        bool validX = x > 0 && x < width;
        bool validY = y > 0 && y < height;
        if (validX && validY){
            return platePixels[y][x];
        }
        return false;
    }



    sf::Color toGreyscale(double n) {
        double nhat = clampf((n + 1)/2.f, 0, 1);
        uint8_t grey = static_cast<uint8_t>(nhat * 255);
        return sf::Color(grey, grey, grey, 255);
    }

};


#endif
