#include "include/Camera.hpp"
#include "include/Wall.hpp"
#include "include/Box.hpp"
#include "include/Constants.hpp"
#include "include/utils.hpp"

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
        float v = Cam.speed * dt;

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
			Cam.speed = 5.0f;
        }
        else {
            Cam.speed = 2.5f;
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

		Cam.setup();
        setupLight();

        drawFloor();
        drawLabyrinth();

        win.display();
    }

    if (gTexWall)  glDeleteTextures(1, &gTexWall);
    if (gTexFloor) glDeleteTextures(1, &gTexFloor);
    return 0;
}
