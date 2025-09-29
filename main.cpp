#include <cmath>
#include <ctime>
#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <algorithm>
#include <corecrt_math_defines.h>
mat<4,4> ModelView, Viewport, Perspective;
constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void rasterize(vec4 clip[3], std::vector<double> &zbuffer, TGAImage &framebuffer, TGAColor color)
{
    vec4 ndc[3]    = { clip[0]/clip[0].w, clip[1]/clip[1].w, clip[2]/clip[2].w };                // normalized device coordinates
    vec2 screen[3] = { (Viewport*ndc[0]).xy(), (Viewport*ndc[1]).xy(), (Viewport*ndc[2]).xy() }; // screen coordinates
    mat<3,3> ABC = {{{screen[0].x, screen[0].y, 1} , {screen[1].x, screen[1].y, 1}, {screen[2].x, screen[2].y, 1}}};
    if(ABC.det() < 1) return;
    mat<3,3> invTransABC{ABC.invert_transpose()};
    auto [BBMinX, BBMaxX] = std::minmax({ABC[0].x, ABC[1].x, ABC[2].x});
    auto [BBMinY, BBMaxY] = std::minmax({ABC[0].y, ABC[1].y, ABC[2].y});
#pragma omp parallel for
    for (int x = std::max<int>(BBMinX, 0); x < std::max<int>(BBMaxX, framebuffer.width() - 1); ++x)
    {
        for (int y = std::max<int>(BBMinY, 0); y < std::max<int>(BBMaxY, framebuffer.height() - 1); ++y)
        {
            vec3 bc = invTransABC * vec3{static_cast<double>(x), static_cast<double>(y), 1};
            if (bc.x < 0 || bc.y < 0|| bc.z < 0) continue;
            double z = bc * vec3{ndc[0].z, ndc[1].z, ndc[2].z};
            if (z < zbuffer[x + framebuffer.width() * y]) continue;
            zbuffer[x + framebuffer.width() * y] = z;
            framebuffer.set(x, y, color);
        }
    }

}

void initViewPoint(const int x, const int y, const int w, const int h)
{
    Viewport = {{{w/2., 0,    0, x + w/2.},
                 {0,    h/2., 0, y + h/2.},
                 {0,    0,    1, 0       },
                 {0,    0,    0, 1       }}};
}

void initPerspective(double f) 
{
    Perspective = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, -1/f, 1}}};
}

//I understand how it translate and then transform the coordinate, but how does it actually affect how the model looks in the model?
void lookout(const vec3 eye, const vec3 center, const vec3 up)
{
    vec3 n = normalize(eye - center);
    vec3 l = normalize(cross(up, n));
    vec3 m = normalize(cross(n, l));
    ModelView = mat<4,4>{{{l.x, l.y, l.z, 0}, {m.x, m.y, m.z, 0}, {n.x, n.y, n.z, 0}, {0, 0, 0, 1}}} 
    * mat<4,4>{{{1, 0, 0, -center.x}, {0, 1, 0, -center.y}, {0, 0, 1, -center.z}, {0, 0, 0, 1}}};
}

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
    lookout(eye, center, up);
    initPerspective(norm(eye - center));
    initViewPoint(width/16, height/16, width*7/8, height*7/8);
    model model{argv[1]};
    TGAImage framebuffer(width, height, TGAImage::RGB);
    std::vector<double> zbuffer(width*height, -std::numeric_limits<double>::max());

    for (int i=0; i<model.nfaces(); i++) { // iterate through all triangles
        vec4 clip[3];
        for (int d : {0, 1, 2})
        {
            vec3 v{model.vert(i, d)};
            clip[d] = Perspective * ModelView * vec4{v.x, v.y, v.z, 1.};//Why is viewport removed from this line?
        }
        TGAColor rnd;
        for (int c=0; c<3; c++) rnd[c] = std::rand()%255;
        rasterize(clip, zbuffer, framebuffer, rnd);
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

