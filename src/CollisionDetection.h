#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include <vector>
#include "Particle.h" // Ensure this matches your Particle class
#include "Table.h"

class CollisionDetection {
public:
    static void resolveClothTableCollisions(std::vector<Particle>& particles, const Table& table);
};

#endif // COLLISIONDETECTION_H
