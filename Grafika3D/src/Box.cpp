#include "../include/Box.hpp"
#include "../include/utils.hpp"

std::vector<Box> gBoxes;
int gBoxesRemaining;

void Box::drawBox() const {
	glPushMatrix();
	glTranslatef((x1 + x2) / 2.0f, (y1 + y2) / 2.0f, (z1 + z2) / 2.0f);
	setupMaterial();
	drawUnitCubeLit(BOX_HEIGT / 2);
	glPopMatrix();
}

void Box::setupMaterial() const {
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

std::ostream& operator<<(std::ostream& os, const COLOR& color) {
	switch (color) {
		case RED:
			os << "RED";
			break;
		case GREEN:
			os << "GREEN";
			break;
		case BLUE:
			os << "BLUE";
			break;
	}
	return os;
}