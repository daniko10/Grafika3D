#pragma once
#include <vector>

struct Box {
    float x1, x2, y1, y2, z1, z2;
    bool collected = false;
};

extern std::vector<Box> gBoxes;
extern int gBoxesRemaining;