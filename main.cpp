#include <iostream>
#include <unistd.h>
#include <SDL/Window.hpp>
#include <SDL/Keyboard.hpp>
#include "CanonicalTree.hpp"


int main() {
    srand(time(0));
    Event event;

    Window win("CellautEngine", 800, 600);

    CanonicalTree t(3, 4);
    std::cout << "tree level: " << t.getLevel() << std::endl;
    t.set(1,1,1,3);

    std::cout << t.get(1,1,1) << std::endl;


    //std::cout << "\033[2J\033[?25l";
    for(long i = 0;!(Keyboard::isKeyPressed(SDLK_ESCAPE) || win.isClosed());i++){



        win.update();
        usleep(100);
    }

    return 0;
}