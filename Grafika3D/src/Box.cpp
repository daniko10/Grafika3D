#include "../include/Box.hpp"
#include "../include/utils.hpp"

std::vector<Box> gBoxes;
int gBoxesRemaining;

void Box::drawBox() const {
	glPushMatrix();
	glTranslatef((x1 + x2) / 2.0f, (y1 + y2) / 2.0f, (z1 + z2) / 2.0f);
	switch (color) {
		case RED:
			setupMaterial_Red();
			break;
		case BLUE:
			setupMaterial_Blue();
			break;
		case GREEN:
			setupMaterial_Green();
			break;
		}
	drawUnitCubeLit(BOX_HEIGT / 2, []() {});
	glPopMatrix();
}