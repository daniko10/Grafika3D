#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

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
float clampf(float , float , float );
unsigned int loadTexture(const std::string&);
void initOpenGL();
void reshapeScreen(sf::Vector2u);
void setupLight();
void drawFloor(int = 6);
void drawUnitCubeLit(float = 0.3f);
void drawLabyrinth();
void initObstacles();