#pragma once
#include <vector>

struct Wall {
    double x1, x2, z1, z2;

    void drawWall() const;
};

extern std::vector<Wall> gWalls;