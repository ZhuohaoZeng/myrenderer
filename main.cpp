#include "mygl.h"
#include "model.h"
#include <iostream>

extern mat<4,4> ModelView, Perspective; // "OpenGL" state matrices and
extern std::vector<double> zbuffer;

struct RandomShader : IShader {
    const model &mo;
    TGAColor color = {};
    vec3 tri[3];  // triangle in eye coordinates

    RandomShader(const model &m) : mo(m) {
    }

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = mo.vert(face, vert);                          // current vertex in object coordinates
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz();                            // in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        return {false, color};                                    // do not discard the pixel
    }
};

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    constexpr int width  = 800;
    constexpr int height = 800;
    constexpr vec3    eye{-2,0,2}; // camera position
    constexpr vec3 center{0,0,0};  // camera direction
    constexpr vec3     up{0,1,0};  // camera up vector
    lookat(eye, center, up);
    initPerspective(norm(eye - center));
    initViewport(width/16, height/16, width*7/8, height*7/8);
    initZbuffer(width, height);
    model model{argv[1]};
    TGAImage framebuffer(width, height, TGAImage::RGB);    
    RandomShader shader(model);
    for (int f=0; f<model.nfaces(); f++) { // iterate through all triangles
        shader.color = {static_cast<unsigned char>(std::rand() % 255),
                        static_cast<unsigned char>(std::rand() % 255),
                        static_cast<unsigned char>(std::rand() % 255)};
        Triangle clip = {shader.vertex(f, 0),  // assemble the primitive
                         shader.vertex(f, 1),
                         shader.vertex(f, 2)};
        rasterize(clip, shader, framebuffer);
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

