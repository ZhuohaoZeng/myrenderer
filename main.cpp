#include "mygl.h"
#include "model.h"
#include "PhongShader.h"
#include "BlinnPhongShader.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    constexpr int width  = 800;
    constexpr int height = 800;
    constexpr vec3  light{ 1, 1, 1};
    constexpr vec3    eye{-1,0,2}; // camera position
    constexpr vec3 center{0,0,0};  // camera direction
    constexpr vec3     up{0,1,0};  // camera up vector
    lookat(eye, center, up);
    initPerspective(norm(eye - center));
    initViewport(width/16, height/16, width*7/8, height*7/8);
    initZbuffer(width, height);
    model model{argv[1]};
    TGAImage framebuffer(width, height, TGAImage::RGB);    
    PhongShader shader(light,  model);
    for (int f=0; f<model.nfaces(); f++) { // iterate through all triangles
        Triangle clip = {shader.vertex(f, 0),  // assemble the primitive
                         shader.vertex(f, 1),
                         shader.vertex(f, 2)};
        rasterize(clip, shader, framebuffer);
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

