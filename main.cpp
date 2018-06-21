#include <iostream>
#include <unistd.h>
#include <SDL/Window.hpp>
#include <SDL/Keyboard.hpp>
#include <OpenGL/Camera.hpp>
#include "CanonicalTree.hpp"


int main() {
    srand(time(0));
    Event event;

    Window win("CellautEngine", 400, 400);

    int L = 9, N = 1<<L;
    CanonicalTree t(L, 0);
    int s = 100;
    for(int j=0;j<100;j++){
        for(int i = 0;i<100;i++){
            t.set(N/2+i,N/2+j,N/2,1);
        }
    }
    std::cout << "tree level: " << t.getLevel() << std::endl;

    double speed = 10;
    Camera camera({N/4, N/2, N/4}, {N/2, N/2, N/2}, {0, 1, 0}, glm::vec3(speed), glm::vec3(0.01));

    win.updateInput();
    Mouse::capturePointer(true);
    int nbFrames = 60, frameCount = 0;
    for(long i = 0;!(Keyboard::isKeyPressed(SDLK_ESCAPE) || win.isClosed());i++){
        win.updateInput();
        camera.move();

        win.setColour(Colour::white());
        win.clear();

        auto start = std::chrono::system_clock::now();
        /*for(int i = 0;i<10*N;i++){
            int x = rand()%N, y = rand()%N;
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
        }*/

        float nbSteps = 400,step = 1/nbSteps;
        int size = win.getWidth()/nbSteps;
        glm::vec2 coord(0,0);
        glm::mat3 MVP = glm::mat3(camera.toMatrix());
        glm::vec3 pos = camera.getPosition();
        if(pos.x<1) pos.x = 1;
        if(pos.y<1) pos.y = 1;
        if(pos.z<1) pos.z = 1;
        if(pos.x>N-1) pos.x = N-1;
        if(pos.y>N-1) pos.y = N-1;
        if(pos.z>N-1) pos.z = N-1;
        camera.setPosition(pos);
        for(coord.y = 0;coord.y<1;coord.y+=step){
            for(coord.x=0;coord.x<1;coord.x+=step){
                glm::vec2 cd = (coord*2.f-1.f)*0.5f;
                glm::vec3 dir = MVP*glm::vec3(-cd.x, cd.y, 1);


                auto ray = t.raycast(pos.x,pos.y,pos.z, dir.x,dir.y,dir.z);
                float dist = ray;
                int c = dist<1?127:12700000/(dist*dist);
                c=c>127?127:c;
                c = 127-log(dist)*10;
                win.setColour(Colour(c*(1+ray.nx),c*(1+ray.ny),c*(1+ray.nz)));
                win.fillRect((coord.x)/step*size,(1-coord.y)/step*size,size,size);
            }
        }

        frameCount++;
        if(frameCount == nbFrames){
            frameCount = 0;

            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> diff = end-start;
            std::cout << diff.count() << "s   " << 1/diff.count() << "fps" << std::endl;
        }

        win.present();
        usleep(100);
    }

    return 0;
}