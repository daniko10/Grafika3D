#pragma once
#include <vector>

enum COLOR {
    RED, GREEN, BLUE
};

struct Box {
    float x1, x2, y1, y2, z1, z2;

    void drawBox() const;
	COLOR color;
    bool collected = false;
};

extern std::vector<Box> gBoxes;
extern int gBoxesRemaining;