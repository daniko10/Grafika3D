#include "utils.hpp"

unsigned int gTexWall = 0, gTexFloor = 0;

void showConsoleStatus() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hOut, pos);

    std::cout << "========== LABIRYNT 3D ==========\n";
    std::cout << "Camera: \n";
    std::cout << "  X: " << Cam.x << "\n";
    std::cout << "  Y: " << Cam.y << "\n";
    std::cout << "  Z: " << Cam.z << "\n";
    std::cout << "(FOV): " << Cam.fov << "\n";
    std::cout << "Left boxes to find: " << gBoxesRemaining << "\n";
    std::cout << "----------------------------------\n";
    std::cout << "Keys:\n";
    std::cout << "  W/S/A/D - movement\n";
    std::cout << "  mouse - camera\n";
    std::cout << "  Ctrl / Spacja - down / up\n";
    std::cout << "  Q / E - FOV\n";
    std::cout << "  Shift - sprint\n";
    std::cout << "  ESC - EXIT\n";
    std::cout << "==================================\n";
}

bool checkBoxCollision(float x, float y, float z) {
    for (auto& b : gBoxes) {
        if (x > b.x1 - MARGIN && x < b.x2 + MARGIN &&
            y < b.y2 + MARGIN &&
            z > b.z1 - MARGIN && z < b.z2 + MARGIN)
        {
            if (!b.collected)
            {
                b.collected = true;
                gBoxesRemaining--;
            }
            return true;
        }
    }
    return false;
}

bool collides(float x, float z, float height_max) {
    for (const auto& w : gWalls) {
        if (x > w.x1 - MARGIN && x < w.x2 + MARGIN &&
            z > w.z1 - MARGIN && z < w.z2 + MARGIN && Cam.y <= height_max)
        {
            return true;
        }
    }

    return checkBoxCollision(x, Cam.y, z);
}