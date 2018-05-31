#include <iostream>
#include <unistd.h>
#include <SDL/Window.hpp>
#include <SDL/Keyboard.hpp>
#include "CanonicalTree.hpp"


int main() {
    srand(time(0));
    Event event;

    Window win("CellautEngine", 800, 600);

    CanonicalTree t(3, 0);
    std::cout << "tree level: " << t.getLevel() << std::endl;
    t.set(0,1,1,1);
    t.set(3,1,1,1);
    t.set(7,1,1,1);

    for(int x = 0;x<8;x++){
        std::cout << "(" << x << ",1,1): " << t.get(x,1,1) << std::endl;
    }

    //std::cout << "\033[2J\033[?25l";
    for(long i = 0;!(Keyboard::isKeyPressed(SDLK_ESCAPE) || win.isClosed());i++){
        win.setColour(Colour::white());
        win.clear();

        win.setColour(Colour::cyan());
        int s = 30;
        for(int x = 0;x<8;x++){
            if(t.get(x,1,1))
                win.drawRect(x*s,0, s, s);
        }


        win.update();
        usleep(100);
    }

    return 0;
}