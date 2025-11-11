#pragma once
#include <vector>
#include <iostream>

enum COLOR {
    RED, GREEN, BLUE
};

std::ostream& operator<<(std::ostream& os, const COLOR& color);

struct Box {
    float x1, x2, y1, y2, z1, z2;

    void drawBox() const;
    void setupMaterial() const;
	COLOR color;
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess[1];

    bool collected = false;

	Box(float x1, float x2, float y1, float y2, float z1, float z2, COLOR color,
		const float* ambient, const float* diffuse, const float* specular, const float* shininess)
		: x1(x1), x2(x2), y1(y1), y2(y2), z1(z1), z2(z2), color(color), collected(false)
	{
		std::copy(ambient, ambient + 4, this->ambient);
		std::copy(diffuse, diffuse + 4, this->diffuse);
		std::copy(specular, specular + 4, this->specular);
		std::copy(shininess, shininess + 1, this->shininess);
	}
};


extern std::vector<Box> gBoxes;
extern int gBoxesRemaining;