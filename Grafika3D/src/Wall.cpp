#include "../include/Wall.hpp"
#include "../include/utils.hpp"

std::vector<Wall> gWalls;

void Wall::drawWall() const
{
    GLfloat ambient[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    GLfloat diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat shine[] = { 10.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);

    glBindTexture(GL_TEXTURE_2D, gTexWall);
    glColor3f(1, 1, 1);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(x1, 0, z1);
    glTexCoord2f(1, 0); glVertex3f(x2, 0, z1);
    glTexCoord2f(1, 1); glVertex3f(x2, WALL_HEIGHT, z1);
    glTexCoord2f(0, 1); glVertex3f(x1, WALL_HEIGHT, z1);

    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(x2, 0, z2);
    glTexCoord2f(1, 0); glVertex3f(x1, 0, z2);
    glTexCoord2f(1, 1); glVertex3f(x1, WALL_HEIGHT, z2);
    glTexCoord2f(0, 1); glVertex3f(x2, WALL_HEIGHT, z2);

    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(x1, 0, z2);
    glTexCoord2f(1, 0); glVertex3f(x1, 0, z1);
    glTexCoord2f(1, 1); glVertex3f(x1, WALL_HEIGHT, z1);
    glTexCoord2f(0, 1); glVertex3f(x1, WALL_HEIGHT, z2);

    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(x2, 0, z1);
    glTexCoord2f(1, 0); glVertex3f(x2, 0, z2);
    glTexCoord2f(1, 1); glVertex3f(x2, WALL_HEIGHT, z2);
    glTexCoord2f(0, 1); glVertex3f(x2, WALL_HEIGHT, z1);

    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(x1, WALL_HEIGHT, z1);
    glTexCoord2f(1, 0); glVertex3f(x2, WALL_HEIGHT, z1);
    glTexCoord2f(1, 1); glVertex3f(x2, WALL_HEIGHT, z2);
    glTexCoord2f(0, 1); glVertex3f(x1, WALL_HEIGHT, z2);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}