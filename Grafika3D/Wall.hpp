#pragma once
#include <vector>

struct Wall {
    float x1, x2, z1, z2;
};

extern std::vector<Wall> gWalls;