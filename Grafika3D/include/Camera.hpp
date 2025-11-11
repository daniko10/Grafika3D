#pragma once
#include <tuple>
#include <cmath>

struct Camera {
    float x = -2.0f, y = 1.0f, z = -2.0f;
    float fi = 0.0f;
    float theta = 0.0f;
    float fov = 60.0f;
    float speed = 2.5f;

    void setup();
    std::tuple<float, float, float> direction();
};

extern Camera Cam;