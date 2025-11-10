#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#include <GL/glu.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#else
#include <GL/glu.h>
#endif

unsigned int gTexWall = 0, gTexFloor = 0;
const float PI = 3.14159265f;
float MOVE_SPEED = 2.5f;
static const float MOUSE_SENS = 0.0009f;
static const float THETA_SCOPE = PI / 3;
static const float THETA_MIN = -THETA_SCOPE, THETA_MAX = THETA_SCOPE;
static const float MARGIN = 0.2f;
static const float WALL_HEIGHT = 2.2f;
static const float BOX_HEIGT = 0.6f;

struct CameraFPS {
    float x = -2.0f, y = 1.0f, z = -2.0f;
    float fi = 0.0f;
    float theta = 0.0f;
	float fov = 60.0f;
} Cam;

struct Wall {
    float x1, x2, z1, z2;
};
std::vector<Wall> gWalls;

int gBoxesRemaining = 0;
struct Box {
    float x1, x2, y1, y2, z1, z2;
    bool collected = false;
};
std::vector<Box> gBoxes;

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

bool collides(float x, float z, float height_max = 2.2) {
    for (const auto& w : gWalls) {
        if (x > w.x1 - MARGIN && x < w.x2 + MARGIN &&
            z > w.z1 - MARGIN && z < w.z2 + MARGIN && Cam.y <= height_max)
        {
            return true;
        }
    }

	return checkBoxCollision(x, Cam.y, z);
}

static inline float clampf(float v, float a, float b) { return (v < a ? a : (v > b ? b : v)); }

unsigned int loadTexture(const std::string& path) {
    sf::Image img;
    if (!img.loadFromFile(path)) {
        std::cerr << "Błąd: nie mogę wczytać tekstury: " << path << "\n";
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

static void initOpenGL() {
    glClearColor(0.12f, 0.13f, 0.16f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
}

static void reshapeScreen(sf::Vector2u size) {
    if (!size.y) size.y = 1;
    glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(Cam.fov, size.x / (double)size.y, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

inline void cameraDir(float fi, float theta, float& dx, float& dy, float& dz) {
    const float cth = std::cos(theta);
    dx = cth * std::sin(fi);
    dy = std::sin(theta);
    dz = -cth * std::cos(fi);
}

static void setupCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float dx, dy, dz; cameraDir(Cam.fi, Cam.theta, dx, dy, dz);
    gluLookAt(Cam.x, Cam.y, Cam.z, Cam.x + dx, Cam.y + dy, Cam.z + dz, 0, 1, 0);
}

static void setupLight() {
    const GLfloat pos[] = { Cam.x, Cam.y, Cam.z, 1.0f };
	const GLfloat amb[] = { 0.35f, 0.35f, 0.35f, 1.0f }; // ogólne oświetlenie
	const GLfloat diff[] = { 1.3f, 1.3f, 1.3f, 1.0f }; // zależne od kąta padania (bokiem - przgasa, z przodu - jasne)
	const GLfloat spec[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // odbicia

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.2f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.45f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.3f);

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
}

static void drawFloor(int half = 1000) {
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

void setupMaterial_Red() {
    GLfloat ambient[] = { 0.3f, 0.1f, 0.1f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.3f, 0.3f, 1.0f };
    GLfloat specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat shininess[] = { 50.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void setupMaterial_Blue() {
    GLfloat ambient[] = { 0.1f, 0.1f, 0.3f, 1.0f };
    GLfloat diffuse[] = { 0.2f, 0.4f, 0.8f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininess[] = { 90.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void setupMaterial_Green() {
    GLfloat ambient[] = { 0.1f, 0.3f, 0.1f, 1.0f };
    GLfloat diffuse[] = { 0.3f, 0.7f, 0.3f, 1.0f };
    GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat shininess[] = { 0.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

static void drawWall(float x1, float x2, float z1, float z2)
{
    const float height = 2.0f;

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
    glTexCoord2f(1, 1); glVertex3f(x2, height, z1);
    glTexCoord2f(0, 1); glVertex3f(x1, height, z1);

    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(x2, 0, z2);
    glTexCoord2f(1, 0); glVertex3f(x1, 0, z2);
    glTexCoord2f(1, 1); glVertex3f(x1, height, z2);
    glTexCoord2f(0, 1); glVertex3f(x2, height, z2);

    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(x1, 0, z2);
    glTexCoord2f(1, 0); glVertex3f(x1, 0, z1);
    glTexCoord2f(1, 1); glVertex3f(x1, height, z1);
    glTexCoord2f(0, 1); glVertex3f(x1, height, z2);

    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(x2, 0, z1);
    glTexCoord2f(1, 0); glVertex3f(x2, 0, z2);
    glTexCoord2f(1, 1); glVertex3f(x2, height, z2);
    glTexCoord2f(0, 1); glVertex3f(x2, height, z1);

    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(x1, height, z1);
    glTexCoord2f(1, 0); glVertex3f(x2, height, z1);
    glTexCoord2f(1, 1); glVertex3f(x2, height, z2);
    glTexCoord2f(0, 1); glVertex3f(x1, height, z2);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}

static const float T = 0.02f;

inline void HWallInit(float x1, float x2, float z) {
    drawWall(x1, x2, z, z - T);
	gWalls.push_back({ x1, x2, z, z - T });
}
inline void VWallInit(float x, float z1, float z2) {
    drawWall(x, x + T, z1, z2);
	gWalls.push_back({ x, x + T, z1, z2 });
}

inline void HWall(float x1, float x2, float z) {
    drawWall(x1, x2, z, z - T);
}
inline void VWall(float x, float z1, float z2) {
    drawWall(x, x + T, z1, z2);
}

static void drawUnitCubeLit(float a = 0.3f, void(*f)() = setupMaterial_Green) {
    glBegin(GL_QUADS);

    f();
    glNormal3f(0, 0, 1);  glVertex3f(-a, -a, +a); glVertex3f(+a, -a, +a); glVertex3f(+a, +a, +a); glVertex3f(-a, +a, +a);
    glNormal3f(0, 0, -1); glVertex3f(-a, -a, -a); glVertex3f(-a, +a, -a); glVertex3f(+a, +a, -a); glVertex3f(+a, -a, -a);
    glNormal3f(1, 0, 0);  glVertex3f(+a, -a, -a); glVertex3f(+a, +a, -a); glVertex3f(+a, +a, +a); glVertex3f(+a, -a, +a);
    glNormal3f(-1, 0, 0); glVertex3f(-a, -a, -a); glVertex3f(-a, -a, +a); glVertex3f(-a, +a, +a); glVertex3f(-a, +a, -a);
    glNormal3f(0, 1, 0);  glVertex3f(-a, +a, -a); glVertex3f(-a, +a, +a); glVertex3f(+a, +a, +a); glVertex3f(+a, +a, -a);
    glNormal3f(0, -1, 0); glVertex3f(-a, -a, -a); glVertex3f(+a, -a, -a); glVertex3f(+a, -a, +a); glVertex3f(-a, -a, +a);
    glEnd();

}

void initObstacles() {
    HWallInit(-5, -2, -5);
    HWallInit(-2, +2, -5);
    HWallInit(+2, 5, -5);

    HWallInit(-5, -2, 5);
    HWallInit(-2, +2, 5);
    HWallInit(+2, 5, 5);

    VWallInit(-5, -5, -2);
    VWallInit(-5, -2, 2);
    VWallInit(-5, 2, 5);

    VWallInit(5, -5, -2);
    VWallInit(5, -2, 2);
    VWallInit(5, 2, 5);

    // Wnętrze

    HWallInit(-1.0f, +1.0f, -1.5f);
    HWallInit(-1.0f, +1.0f, +0.5f);
    VWallInit(-1.0f, -1.5f, +0.5f);
    VWallInit(+1.0f - T, -1.5f, +0.5f);

    HWallInit(-5.0f, -1.0f, +1.5f);
    HWallInit(-4.0f, -1.0f, 3.0f);
    VWallInit(-1.0f, +0.5f, 3.0f);

    HWallInit(+1.5f, +3.5f, -0.5f);
    VWallInit(+3.5f - T, -0.5f, +0.7f);
    HWallInit(+1.5f, +3.5f, +0.7f);

    HWallInit(-4.0f, -2.0f, -3.0f);
    VWallInit(-4.0f, -3.0f, -1.0f);
    HWallInit(-4.0f, -2.0f, -1.0f);
    VWallInit(-2.0f, -1.0f, 0.0f);
    VWallInit(-2.0f, -3.0f, -5.0f);

    HWallInit(+2.0f, +4.0f, +3.5f);
    VWallInit(+2.0f, +3.5f, +5.0f);

	gBoxes.push_back({ -2.8f - 0.3f, -2.8f + 0.3f, 0.0f, 0.6f, 2.2f - 0.3f, 2.2f + 0.3f });
	gBoxes.push_back({ 3.2f - 0.3f, 3.2f + 0.3f, 0.0f, 0.6f, 4.4f - 0.3f, 4.4f + 0.3f });
	gBoxes.push_back({ 3.0f - 0.3f, 3.0f + 0.3f, 0.0f, 0.6f, 0.1f - 0.3f, 0.1f + 0.3f });
    
    gBoxesRemaining = gBoxes.size();
}

static void drawLabyrinth() {
    // Obramówka
    HWall(-5, -2, -5);
    HWall(-2, +2, -5);
    HWall(+2, 5, -5);

    HWall(-5, -2, 5);
    HWall(-2, +2, 5);
    HWall(+2, 5, 5);

    VWall(-5, -5, -2);
    VWall(-5, -2, 2);
    VWall(-5, 2, 5);
    
    VWall(5, -5, -2);
    VWall(5, -2, 2);
    VWall(5, 2, 5);

	// Wnętrze

    HWall(-1.0f, +1.0f, -1.5f);
    HWall(-1.0f, +1.0f, +0.5f);
    VWall(-1.0f, -1.5f, +0.5f);
    VWall(+1.0f - T, -1.5f, +0.5f);

    HWall(- 5.0f, -1.0f, +1.5f);
    HWall(-4.0f, -1.0f, 3.0f);
    VWall(-1.0f, +0.5f, 3.0f);

    HWall(+1.5f, +3.5f, -0.5f);
    VWall(+3.5f - T, -0.5f, +0.7f);
    HWall(+1.5f, +3.5f, +0.7f);

    HWall(-4.0f, -2.0f, -3.0f);
    VWall(-4.0f, -3.0f, -1.0f);
    HWall(-4.0f, -2.0f, -1.0f);
    VWall(-2.0f, -1.0f, 0.0f);
	VWall(-2.0f, -3.0f, -5.0f);

    HWall(+2.0f, +4.0f, +3.5f);
    VWall(+2.0f, +3.5f, +5.0f);

    glPushMatrix(); glTranslatef(-2.8f, 0.3f, +2.2f); drawUnitCubeLit(0.3f, setupMaterial_Red);   glPopMatrix();
    glPushMatrix(); glTranslatef(+3.2f, 0.3f, +4.4f); drawUnitCubeLit(0.3f, setupMaterial_Blue);  glPopMatrix();
    glPushMatrix(); glTranslatef(+3.0f, 0.3f, 0.1f); drawUnitCubeLit(0.3f, setupMaterial_Green); glPopMatrix();
}

int main() {
    sf::ContextSettings s; s.depthBits = 24; s.stencilBits = 8; s.majorVersion = 2; s.minorVersion = 1;
    sf::RenderWindow win(sf::VideoMode(1024, 768), "Labiryncik", sf::Style::Default, s);
    win.setVerticalSyncEnabled(true);
    win.setMouseCursorGrabbed(true);
    win.setMouseCursorVisible(false);

    initOpenGL();
    initObstacles();

    gTexWall = loadTexture("textures/wall.jpg");
    gTexFloor = loadTexture("textures/floor.jpg");

    if (!gTexWall || !gTexFloor) std::cerr << "Błąd z wczytywaniem tekstur.\n";

    sf::Vector2i center((int)win.getSize().x / 2, (int)win.getSize().y / 2);
    sf::Mouse::setPosition(center, win);
    sf::Clock dtClock;

    bool running = true;
    while (running) {
        reshapeScreen(win.getSize());
        for (sf::Event e; win.pollEvent(e); ) {
            if (e.type == sf::Event::Closed) running = false;
            if (e.type == sf::Event::Resized) reshapeScreen(win.getSize());
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) running = false;
        }

        sf::Vector2i m = sf::Mouse::getPosition(win);
        Cam.fi += (m.x - center.x) * MOUSE_SENS;
        Cam.theta -= (m.y - center.y) * MOUSE_SENS;
        Cam.theta = clampf(Cam.theta, THETA_MIN, THETA_MAX);
        sf::Mouse::setPosition(center, win);

        float dt = dtClock.restart().asSeconds();
        float v = MOVE_SPEED * dt;

        float fwdx = std::sin(Cam.fi), fwdz = -std::cos(Cam.fi);
        float sidx = std::cos(Cam.fi), sidz = std::sin(Cam.fi);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            float newX = Cam.x + fwdx * v;
            float newZ = Cam.z + fwdz * v;
            if (!collides(newX, newZ)) { Cam.x = newX; Cam.z = newZ; }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            float newX = Cam.x - fwdx * v;
            float newZ = Cam.z - fwdz * v;
            if (!collides(newX, newZ)) { Cam.x = newX; Cam.z = newZ; }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            float newX = Cam.x - sidx * v;
            float newZ = Cam.z - sidz * v;
            if (!collides(newX, newZ)) { Cam.x = newX; Cam.z = newZ; }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            float newX = Cam.x + sidx * v;
            float newZ = Cam.z + sidz * v;
            if (!collides(newX, newZ)) { Cam.x = newX; Cam.z = newZ; }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
			MOVE_SPEED = 5.0f;
        }
        else {
            MOVE_SPEED = 2.5f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            float newY = Cam.y - v;
            if (newY > 0.1f && !checkBoxCollision(Cam.x, newY, Cam.z))
                Cam.y = newY;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) { if (Cam.y + v > 10.0) Cam.y = 10.0; else Cam.y += v; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) { Cam.fov -= v; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) { Cam.fov += v; }

        showConsoleStatus();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setupCamera();
        setupLight();

        drawFloor(6);
        drawLabyrinth();

        win.display();
    }

    if (gTexWall)  glDeleteTextures(1, &gTexWall);
    if (gTexFloor) glDeleteTextures(1, &gTexFloor);
    return 0;
}
