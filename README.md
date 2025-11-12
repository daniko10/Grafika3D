# Labiryncik

## Autor
**Daniel Czapla**  
Kierunek: Informatyka  
Grupa: mgr.inz Jakub Hajduga, środa 16:45  

---

## Opis projektu

**Labiryncik** to projekt interaktywnej aplikacji trójwymiarowej, łączącej możliwości biblioteki **SFML** z niskopoziomowym renderowaniem przy pomocy **OpenGL**.  
Użytkownik steruje kamerą w trybie pierwszoosobowym  i eksploruje labirynt zbudowany ze ścian i tekstur. Użytkownik jest w stanie widzieć przeszkody dzięki latarce zamontowanej na jego głowie (kamerze).
Celem rozgrywki jest **odnalezienie i zebranie wszystkich kolorowych boxów** rozmieszczonych w przestrzeni.

---

## Najważniejsze funkcjonalności

### Sterowanie i interakcja
- **Ruch FPS** – sterowanie klawiszami `W`, `S`, `A`, `D`.
- **Mysz** – obrót kamery w osi poziomej i pionowej, THETA ograniczona do +- 60st.
- **Latanie / opadanie** – `Spacja` / `Lewy Ctrl`.
- **Sprint** – `Lewy Shift`.
- **Regulacja pola widzenia (FOV)** – `Q` / `E`.
- **Wybór boxa do edytcji** – `1` (czerwony), `2` (niebieski), `3` (zielony).
- **Zmiana połysku wybranego pudełka** – strzałki `↑` / `↓`.
- **Wyjście z gry** – `ESC`.

### Efekty graficzne i fizyka
- Dynamiczne oświetlenie `GL_LIGHT0` zależne od kierunku patrzenia kamery, latarka zamontowana na kamerze, siła światła słabnie w miarę oddalania się od obiektu.  
- Materiały (`ambient`, `diffuse`, `specular`, `shininess`) konfigurowane dla każdego obiektu.  
- Kolizje z ścianami i pudełkami (nie można przez nie przenikać).  
- Zebranie pudełka powoduje zmniejszenie licznika, który odpowiedzialny jest za liczebność nieznalezionych boxów.  
- Możliwość modyfikacji połysku (shininess) w locie.
- W konsoli na bieżąco prezentowane są informacje:
  - pozycja kamery (x, y, z),
  - aktualne FOV,
  - liczba pozostałych pudełek do zebrania,
  - przypomnienie sterowania.

### Obiekty w świecie gry
- **Ściany (`Wall`)** – teksturowane płaszczyzny tworzące labirynt - textures/wall.jpg
- **Pudełka (`Box`)** – kolorowe obiekty 3D do zebrania (RED, GREEN, BLUE).
- **Podłoga (`drawFloor`)** – teksturowany kwadratowy obszar oświetlany w czasie rzeczywistym - textures/floor.jpg

---

## Struktura kodu
**Grafika3D.cpp** odpowiedzalny za:
- tworzenie okna SFML z kontekstem OpenGL
- inicjalizację OpenGL
- wczytywanie tekstur
- inicjalizację labiryntu i pudełek
- główną pętlę zdarzeń i renderingu:
  - obsługę ruchu kamery i kolizji
  - sterowanie prędkością, FOV i materiałami
  - renderowanie sceny

**Camera.hpp / Camera.cpp** odpowiedzialne za:
Definicja struktury Camera:
- współrzędne położenia (x, y, z)
- kąty obrotu (fi, theta)
- prędkość ruchu i pole widzenia (speed, fov)

Metody:
- direction() – zwraca wektor kierunku patrzenia,
- setup() – ustawia macierz widoku przy pomocy gluLookAt()

**Box.hpp / Box.cpp** odpowiedzialne za:
Definicja struktury Box, reprezentującej kolorowe sześciany:
- współrzędne ograniczające (x1, x2, y1, y2, z1, z2)
- kolor (enum COLOR)
- właściwości materiału (ambient, diffuse, specular, shininess)
- metoda drawBox() rysująca obiekt w scenie
- metoda setupMaterial() konfigurująca parametry materiału w OpenGL
- Każde pudełko można „zebrać” po wejściu w kolizję (zmienna collected)

**Wall.hpp / Wall.cpp** odpowiedzialne za:
Struktura Wall opisuje prostokątne ściany labiryntu.
Metoda drawWall():
- rysuje ściany jako bryły o zadanych współrzędnych
- stosuje teksturę ściany (gTexWall)
- nadaje im oświetlenie materiałowe

**utils.hpp / utils.cpp** odpowiedzialne za:
Zbiór funkcji pomocniczych:
- initOpenGL() – inicjalizacja środowiska 3D (światło, głębia, cieniowanie)
- reshapeScreen() – dostosowanie projekcji do rozmiaru okna
- setupLight() – konfiguracja źródła światła
- drawFloor() – rysowanie teksturowanej podłogi
- drawUnitCubeLit() – sześcian z normalami do oświetlenia
- drawLabyrinth() – renderowanie wszystkich ścian i pudełek
- initObstacles() – generowanie geometrii labiryntu i rozmieszczenie pudełek
- collides() i checkBoxCollision() – detekcja kolizji
- loadTexture() – ładowanie plików JPG jako tekstur
- showConsoleStatus() – aktualizacja informacji w konsoli

**Constants.hpp / Constants.cpp** odpowiedzialne za:
Zawiera globalne stałe projektu, takie jak:
- PI
- MOUSE_SENS – czułość myszy
- THETA_SCOPE, THETA_MIN, THETA_MAX – ograniczenia kąta patrzenia
- MARGIN – margines kolizji
- WALL_HEIGHT, BOX_HEIGT, T, DELTA_SHININESS – parametry geometryczne i wizualne

---

## Biblioteki
- SFML/Window.hpp – tworzenie okna, obsługa zdarzeń, myszki i klawiatury
- SFML/Graphics.hpp – ładowanie tekstur i obrazów
- SFML/OpenGL.hpp – integracja z OpenGL
- OpenGL (GL/gl.h, GLU/glu.h) – renderowanie 3D
- Windows.h – funkcje konsolowe i obsługa kursora (tylko w Windows)
- vector, cmath, tuple, optional, iostream

---

## Wymagania:
- Kompilator C++17 (lub nowszy)
- Zainstalowana biblioteka **SFML (>= 2.5)**
- Obsługa **OpenGL** oraz **GLU**
- System: Windows / Linux

---

## Instrukcja uruchomienia

- git clone https://github.com/daniko10/Grafika3D.git
- Otwórz Visual Studio
- Plik -> Otwórz -> Projekt lub rozwiązanie i wskaż na Grafika3D.sln
- Pobierz SFML oraz ustaw standard C++ na >= 17
- Start Debugging





