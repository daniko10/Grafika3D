#pragma once

#include <iostream> 
#include "Camera.hpp"
#include "Wall.hpp"
#include "Box.hpp"
#include "Constants.hpp"

#ifdef _WIN32
#include <windows.h>
#include <GL/glu.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#else
#include <GL/glu.h>
#endif

extern unsigned int gTexWall, gTexFloor;

void showConsoleStatus();
bool checkBoxCollision(float , float , float );
bool collides(float , float , float = 2.2);