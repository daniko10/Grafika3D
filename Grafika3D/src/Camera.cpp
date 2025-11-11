#include "../include/Camera.hpp"
#include "../include/utils.hpp"
Camera Cam;

std::tuple<float, float, float> Camera::direction() {
	float cth = std::cos(theta);
	float dx = cth * std::sin(fi);
	float dy = std::sin(theta);
	float dz = -cth * std::cos(fi);
	return std::make_tuple(dx, dy, dz);
}

void Camera::setup() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float dx, dy, dz;
	std::tie(dx, dy, dz) = direction();
	gluLookAt(x, y, z,
			  x + dx, y + dy, z + dz,
			  0, 1, 0);
}