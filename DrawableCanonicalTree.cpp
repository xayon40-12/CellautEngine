//
// Created by Nathan Touroux on 22/06/2018.
//

#include "DrawableCanonicalTree.hpp"

DrawableCanonicalTree::DrawableCanonicalTree(int level, int value) : CanonicalTree(level, value) {

}

DrawableCanonicalTree::DrawableCanonicalTree(int level, int value, float width) : CanonicalTree(level, value, width) {

}

DrawableCanonicalTree::DrawableCanonicalTree(int level, int value, float width, float px, float py, float pz)
        : CanonicalTree(level, value, width, px, py, pz) {

}

DrawableCanonicalTree::DrawableCanonicalTree(const CanonicalTree &tree) : CanonicalTree(tree) {

}

DrawableCanonicalTree::~DrawableCanonicalTree() {
    delete texture;
}

void DrawableCanonicalTree::initCL(unsigned int width, unsigned int height) {
    if(cl.init()){
        clWidth = width;
        clHeight = height;
        texture = new Texture(width, height);
        texture->load();

        cl.addMem(nodes, "r");//0
        cl.addMem(length, "r");//1
        cl.addMem(topID, "r");//2
        cl.addMem(X, "r");//3
        cl.addMem(Y, "r");//4
        cl.addMem(Z, "r");//5
        cl.addMem(pos, "r");//6
        cl.addTexture(*texture, "w");

        cl.buildProgram("kernel.cl", "render");

        enableCL = true;

    }
}

void DrawableCanonicalTree::draw(Camera &camera) {
    if(!enableCL){
        std::cout << "ERROR: try to draw with OpenCL which is not initialised" << std::endl;
        return;
    }

    glm::vec2 coord(0,0);
    glm::mat3 MVP = glm::mat3(camera.toMatrix());
    pos = camera.getPosition();
    if(pos.x<1) pos.x = 1;
    if(pos.y<1) pos.y = 1;
    if(pos.z<1) pos.z = 1;
    if(pos.x>length-1) pos.x = length-1;
    if(pos.y>length-1) pos.y = length-1;
    if(pos.z>length-1) pos.z = length-1;
    camera.setPosition(pos);

    glm::vec3 X = MVP[0], Y = MVP[1], Z = MVP[2];
    cl.setMem(3, &X);
    cl.setMem(4, &Y);
    cl.setMem(5, &Z);
    cl.setMem(6, &pos);

    cl.compute({clWidth,clHeight});
}

void DrawableCanonicalTree::draw(Camera &camera, Window &win, int sizePixels) {
    int size = sizePixels;
    glm::vec2 coord(0,0);
    glm::mat3 MVP = glm::mat3(camera.toMatrix());
    pos = camera.getPosition();
    if(pos.x<1) pos.x = 1;
    if(pos.y<1) pos.y = 1;
    if(pos.z<1) pos.z = 1;
    if(pos.x>length-1) pos.x = length-1;
    if(pos.y>length-1) pos.y = length-1;
    if(pos.z>length-1) pos.z = length-1;
    camera.setPosition(pos);

    float step = (float)sizePixels/win.getWidth();
    for(coord.y = 0;coord.y<1;coord.y+=step){
        for(coord.x=0;coord.x<1;coord.x+=step){
            glm::vec2 cd = (coord*2.f-1.f)*0.5f;
            glm::vec3 dir = MVP*glm::vec3(-cd.x, cd.y, 1);// = -cd.x*X + cd.y*Y + Z;


            auto ray = raycast(pos.x,pos.y,pos.z, dir.x,dir.y,dir.z);
            float dist = ray;
            int c = dist<1?127:12700000/(dist*dist);
            c=c>127?127:c;
            c = 127-log(dist)*10;
            win.setColour(Colour(c*(1+ray.nx),c*(1+ray.ny),c*(1+ray.nz)));
            win.fillRect((coord.x)/step*size,(1-coord.y)/step*size,size,size);
        }
    }
}

Texture *DrawableCanonicalTree::getTexture() {
    return texture;
}
