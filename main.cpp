#include <iostream>
#include <unistd.h>
#include <SDL/Window.hpp>
#include <SDL/Keyboard.hpp>
#include "CanonicalTree.hpp"


int main() {
    srand(time(0));
    Event event;

    Window win("CellautEngine", 800, 700);

    int L = 9, N = 1<<L;
    CanonicalTree t(L, 0);
    std::cout << "tree level: " << t.getLevel() << std::endl;

    for(long i = 0;!(Keyboard::isKeyPressed(SDLK_ESCAPE) || win.isClosed());i++){
        win.setColour(Colour::white());
        win.clear();

        auto start = std::chrono::system_clock::now();
        for(int i = 0;i<10*N;i++){
            int x = rand()%(N/2), y = rand()%(N/2);
            t.setf(x,y,0,t.getf(x,y,0)+1);
        }
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end-start;
        if(diff.count() > 1){
            std::cout << diff.count() << "s   ";
            std::cout << "nb nodes: " << (t.nbNodes()) << "   ram: " << ((t.nbNodes()* sizeof(Node))>>20) << " MB" << std::endl;
        }

        int s = 1;
        for(int y = 0;y<N;y++) {
            for (int x = 0; x < N; x++) {
                int c = 10*t.getf(x, y, 0);
                if (c){
                    win.setColour(Colour(0,c%256,c%256));
                    win.fillRect(x * s, y*s, s, s);
                }
            }
        }


        win.update();
        usleep(100);
    }

    return 0;
}