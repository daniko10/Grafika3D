#include "../include/utils.hpp"

unsigned int gTexWall = 0, gTexFloor = 0;
std::optional<COLOR> currentBoxModify = std::nullopt;

void showConsoleStatus() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };

    SetConsoleCursorPosition(hOut, pos);

    bool upNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    bool downNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    static bool prevUp = false;
    static bool prevDown = false;

    if ((!upNow && prevUp) || (!downNow && prevDown)) {
        system("cls");
    }

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
    std::cout << "  1 / 2 / 3 - modify RED / BLUE / GREEN box\n";
    std::cout << "  UP / DOWN - increase / decrease shininess of selected box\n";
    std::cout << "  ESC - EXIT\n";

    if (currentBoxModify.has_value())
    {
        for (auto& b : gBoxes) {
            if (b.color == currentBoxModify.value()) {
                if (upNow)
                {
                    std::cout << b.color << " shininess: " << b.shininess[0] << "\n";
                    b.shininess[0] += DELTA_SHININESS;
                    if (b.shininess[0] > 128.0f) b.shininess[0] = 128.0f;
                }
                if (downNow)
                {
                    std::cout << b.color << " shininess: " << b.shininess[0] << "\n";
                    b.shininess[0] -= DELTA_SHININESS;
                    if (b.shininess[0] < 0.0f) b.shininess[0] = 0.0f;
                }
            }
        }
    }

    prevUp = upNow;
    prevDown = downNow;

    std::cout << "\n==================================\n";
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

float clampf(float v, float a, float b) { return (v < a ? a : (v > b ? b : v)); }

unsigned int loadTexture(const std::string& path) {
    sf::Image img;
    if (!img.loadFromFile(path)) {
        std::cerr << "B³¹d: nie mogê wczytaæ tekstury: " << path << "\n";
        return 0;
    }
    img.flipVertically();
    unsigned int id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        img.getSize().x, img.getSize().y, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

void initOpenGL() {
    glClearColor(0.12f, 0.13f, 0.16f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
}

void reshapeScreen(sf::Vector2u size) {
    if (!size.y) size.y = 1;
    glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(Cam.fov, size.x / (double)size.y, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void setupLight() {
    float fwdx = std::sin(Cam.fi);
    float fwdz = -std::cos(Cam.fi);

    const GLfloat pos[] = {
        Cam.x - fwdx * 0.5f,
        Cam.y,
        Cam.z - fwdz * 0.5f,
        1.0f
    };
    const GLfloat amb[] = { 0.35f, 0.35f, 0.35f, 1.0f }; // ogólne oœwietlenie
    const GLfloat diff[] = { 1.3f, 1.3f, 1.3f, 1.0f }; // zale¿ne od k¹ta padania (bokiem - przgasa, z przodu - jasne)
    const GLfloat spec[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // odbicia

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.3f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.45f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.3f);

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
}

void drawFloor(int half) {
    GLfloat floor_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat floor_diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat floor_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat floor_shine[] = { 10.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, floor_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, floor_shine);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, gTexFloor);
    glBegin(GL_QUADS);

    for (int x = -half; x < half; ++x) {
        for (int z = -half; z < half; ++z) {
            glNormal3f(0.0f, 1.0f, 0.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(x, 0.0f, z);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 1.0f, 0.0f, z);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 1.0f, 0.0f, z + 1.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(x, 0.0f, z + 1.0f);
        }
    }

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawUnitCubeLit(float a) {
    glBegin(GL_QUADS);

    glNormal3f(0, 0, 1);  glVertex3f(-a, -a, +a); glVertex3f(+a, -a, +a); glVertex3f(+a, +a, +a); glVertex3f(-a, +a, +a);
    glNormal3f(0, 0, -1); glVertex3f(-a, -a, -a); glVertex3f(-a, +a, -a); glVertex3f(+a, +a, -a); glVertex3f(+a, -a, -a);
    glNormal3f(1, 0, 0);  glVertex3f(+a, -a, -a); glVertex3f(+a, +a, -a); glVertex3f(+a, +a, +a); glVertex3f(+a, -a, +a);
    glNormal3f(-1, 0, 0); glVertex3f(-a, -a, -a); glVertex3f(-a, -a, +a); glVertex3f(-a, +a, +a); glVertex3f(-a, +a, -a);
    glNormal3f(0, 1, 0);  glVertex3f(-a, +a, -a); glVertex3f(-a, +a, +a); glVertex3f(+a, +a, +a); glVertex3f(+a, +a, -a);
    glNormal3f(0, -1, 0); glVertex3f(-a, -a, -a); glVertex3f(+a, -a, -a); glVertex3f(+a, -a, +a); glVertex3f(-a, -a, +a);
    glEnd();

}

void drawLabyrinth() {
    for (const Wall& w : gWalls) {
        w.drawWall();
    }
    for (const Box& b : gBoxes) {
		b.drawBox();
	}
}

void initObstacles() {
    gWalls.clear();

    gWalls.push_back({ -5, -2, -5, -5 - T });
    gWalls.push_back({ -2, +2, -5, -5 - T });
    gWalls.push_back({ +2, 5, -5, -5 - T });

    gWalls.push_back({ -5, -2, 5, 5 - T });
    gWalls.push_back({ -2, +2, 5, 5 - T });
    gWalls.push_back({ +2, 5, 5, 5 - T });

    gWalls.push_back({ -5, -5 + T, -5, -2 });
    gWalls.push_back({ -5, -5 + T, -2, 2 });
    gWalls.push_back({ -5, -5 + T, 2, 5 });

    gWalls.push_back({ 5, 5 + T, -5, -2 });
    gWalls.push_back({ 5, 5 + T, -2, 2 });
    gWalls.push_back({ 5, 5 + T, 2, 5 });

    gWalls.push_back({ -1, 1, -1.5, -1.5 - T });
    gWalls.push_back({ -1, 1, 0.5, 0.5 - T });
    gWalls.push_back({ -1, -1 + T, -1.5, 0.5 });
    gWalls.push_back({ 1 - T, 1, -1.5, 0.5 });

    gWalls.push_back({ -5, -1, 1.5, 1.5 - T });
    gWalls.push_back({ -4, -1, 3, 3 - T });
    gWalls.push_back({ -1, -1 + T, 0.5, 3 });

    gWalls.push_back({ 1.5, 3.5, -0.5, -0.5 - T });
    gWalls.push_back({ 3.5 - T, 3.5, -0.5, 0.7 });
    gWalls.push_back({ 1.5, 3.5, 0.7, 0.7 - T });

    gWalls.push_back({ -4, -2, -3, -3 - T });
    gWalls.push_back({ -4, -4 + T, -3, -1 });
    gWalls.push_back({ -4, -2, -1, -1 - T });
    gWalls.push_back({ -2, -2 + T, -1, 0 });
    gWalls.push_back({ -2, -2 + T, -3, -5 });

    gWalls.push_back({ 2, 4, 3.5, 3.5 - T });
    gWalls.push_back({ 2, 2 + T, 3.5, 5 });

    GLfloat RED_ambient[] = { 0.3f, 0.1f, 0.1f, 1.0f }; // widoczny nawet w cieniu
    GLfloat RED_diffuse[] = { 0.8f, 0.3f, 0.3f, 1.0f }; // g³ówny kolor obiektu
	GLfloat RED_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // odbicia œwiat³a
	GLfloat RED_shininess[] = { 50.0f }; // po³ysk (0 - matowy, 128 - bardzo b³yszcz¹cy)

    GLfloat BLUE_ambient[] = { 0.1f, 0.1f, 0.3f, 1.0f };
    GLfloat BLUE_diffuse[] = { 0.2f, 0.4f, 0.8f, 1.0f };
    GLfloat BLUE_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat BLUE_shininess[] = { 90.0f };

    GLfloat GREEN_ambient[] = { 0.1f, 0.3f, 0.1f, 1.0f };
    GLfloat GREEN_diffuse[] = { 0.3f, 0.7f, 0.3f, 1.0f };
    GLfloat GREEN_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat GREEN_shininess[] = { 0.0f };

    gBoxes.push_back({ -2.8f - 0.3f, -2.8f + 0.3f, 0.0f, 0.6f, 2.2f - 0.3f, 2.2f + 0.3f, RED, RED_ambient, RED_diffuse, RED_specular, RED_shininess });
    gBoxes.push_back({ 3.2f - 0.3f, 3.2f + 0.3f, 0.0f, 0.6f, 4.4f - 0.3f, 4.4f + 0.3f, BLUE, BLUE_ambient, BLUE_diffuse, BLUE_specular, BLUE_shininess });
    gBoxes.push_back({ 3.0f - 0.3f, 3.0f + 0.3f, 0.0f, 0.6f, 0.1f - 0.3f, 0.1f + 0.3f, GREEN, GREEN_ambient, GREEN_diffuse, GREEN_specular, GREEN_shininess });

    gBoxesRemaining = gBoxes.size();
}