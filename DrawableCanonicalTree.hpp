//
// Created by Nathan Touroux on 22/06/2018.
//

#ifndef CELLAUTENGINE_DRAWABLECANONICALTREE_HPP
#define CELLAUTENGINE_DRAWABLECANONICALTREE_HPP


#include <OpenCL/OpenCL_GL.hpp>
#include "CanonicalTree.hpp"
#include "SDL/Window.hpp"
#include <OpenGL/Camera.hpp>

class DrawableCanonicalTree: public CanonicalTree{
private:
    OpenCL_GL cl;
    bool enableCL = false;
    glm::vec3 X,Y,Z,pos;
    unsigned int clWidth = 0, clHeight = 0;

    Texture *texture = nullptr;

public:
    explicit DrawableCanonicalTree(int level = 0, int value = 0);
    //create a tree of indicated level with all same values
    DrawableCanonicalTree(int level, int value, float width);
    DrawableCanonicalTree(int level, int value, float width, float px, float py, float pz);
    DrawableCanonicalTree(const CanonicalTree &tree);
    ~DrawableCanonicalTree();

    void initCL(unsigned int width, unsigned int height);

    void draw(Camera &camera);//draw with OpenCL and OpenGL interop, render in the texture
    void draw(Camera &camera, Window &win, int sizePixels = 4);//draw with CPU raycast and render in the window

    Texture* getTexture();
};


#endif //CELLAUTENGINE_DRAWABLECANONICALTREE_HPP
