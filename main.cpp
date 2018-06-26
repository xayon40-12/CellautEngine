#include <iostream>
#include <unistd.h>
#include <SDL/Window.hpp>
#include <SDL/Keyboard.hpp>
#include <OpenGL/Camera.hpp>
#include <OpenCL/OpenCL.hpp>
#include <OpenGL/Object.hpp>
#include <OpenGL/GLWindow.hpp>
#include "CanonicalTree.hpp"
#include "DrawableCanonicalTree.hpp"

void draw(CanonicalTree &t, Window &win, Camera &camera, OpenCL &cl);

int main() {
    std::cout << RAND_MAX << std::endl;
    srand(time(0));
    Event event;

    GLWindow win("CellautEngine", 1440, 900, true, 3, 3);
    win.setFullscreen(true);

    int L = 9, N = 1<<L;
    DrawableCanonicalTree t(L);
    for (int i = 0; i < 10*N; i++) {
        t.set(rand()%N, rand()%N, rand()%N, rand()%30000);
    }

    std::cout << "tree level: " << t.getLevel() << std::endl;

    double speed = 10;
    Camera camera({N/4, N/2, N/4}, {N/2, N/2, N/2}, {0, 1, 0}, glm::vec3(speed), glm::vec3(0.01));
    Object screen = Object::createSquare({0, 0, 0}, {1, 0, 0}, {0, 1, 0}, "screen.vert", "", "screen.frag");
    screen.create();

    win.updateInput();
    Mouse::capturePointer(true);
    float ratio = (float)win.getWidth()/win.getHeight();
    int size = 200;
    t.initCL(size*ratio, size);
    int frameCount = 0;
    auto start = std::chrono::system_clock::now();
    for(long i = 0;!(Keyboard::isKeyPressed(SDLK_ESCAPE) || win.isClosed());i++){
        win.updateInput();
        camera.move();

        screen.use();
        t.draw(camera);
        t.getTexture()->use();
        screen.show();

        t.getTexture()->remove();
        screen.remove();

        frameCount++;
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end-start;
        double count = diff.count();
        if(count > 1){
            std::cout << frameCount/count << "fps" << std::endl;
            start = std::chrono::system_clock::now();
            frameCount = 0;
        }

        win.present();
        usleep(100);
    }

    return 0;
}