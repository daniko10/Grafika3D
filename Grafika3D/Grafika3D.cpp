#include "include/Camera.hpp"
#include "include/Wall.hpp"
#include "include/Box.hpp"
#include "include/Constants.hpp"
#include "include/utils.hpp"

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.majorVersion = 2;
    settings.minorVersion = 1;

    sf::RenderWindow window(
        sf::VideoMode(1024, 768),
        "Labiryncik",
        sf::Style::Default,
        settings
    );

    window.setVerticalSyncEnabled(true);
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);

    initOpenGL();
    initObstacles();

    gTexWall = loadTexture("textures/wall.jpg");
    gTexFloor = loadTexture("textures/floor.jpg");

    if (!gTexWall || !gTexFloor)
        std::cerr << "Błąd z wczytywaniem tekstur\n";

    sf::Vector2i center(
        static_cast<int>(window.getSize().x / 2),
        static_cast<int>(window.getSize().y / 2)
    );
    sf::Mouse::setPosition(center, window);

	COLOR currentBoxModify = RED;
    sf::Clock deltaClock;
    bool running = true;

    while (running) {
        reshapeScreen(window.getSize());

        for (sf::Event event; window.pollEvent(event); ) {
            if (event.type == sf::Event::Closed)
                running = false;
            if (event.type == sf::Event::Resized)
                reshapeScreen(window.getSize());
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                running = false;
        }

        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        Cam.fi += (mouse.x - center.x) * MOUSE_SENS;
        Cam.theta -= (mouse.y - center.y) * MOUSE_SENS;
        Cam.theta = clampf(Cam.theta, THETA_MIN, THETA_MAX);
        sf::Mouse::setPosition(center, window);

        float dt = deltaClock.restart().asSeconds();
        float v = Cam.speed * dt;

        float fwdx = std::sin(Cam.fi);
        float fwdz = -std::cos(Cam.fi);
        float sidx = std::cos(Cam.fi);
        float sidz = std::sin(Cam.fi);

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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            Cam.speed = 5.0f;
        else
            Cam.speed = 2.5f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            float newY = Cam.y - v;
            if (newY > 0.1f && !checkBoxCollision(Cam.x, newY, Cam.z))
                Cam.y = newY;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (Cam.y + v > 10.0f) Cam.y = 10.0f;
            else Cam.y += v;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) Cam.fov -= v;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) Cam.fov += v;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
			currentBoxModify = RED;
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
			currentBoxModify = BLUE;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
			currentBoxModify = GREEN;
		}
        
        modifyBoxShininess(currentBoxModify);
        showConsoleStatus();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Cam.setup();
        setupLight();

        drawFloor();
        drawLabyrinth();

        window.display();
    }

    if (gTexWall)  glDeleteTextures(1, &gTexWall);
    if (gTexFloor) glDeleteTextures(1, &gTexFloor);

    return 0;
}
