#include "mygl.h"


mat<4,4> ModelView, Viewport, Perspective;
std::vector<double> zbuffer;

vec3 persp(vec3 in)
{
    constexpr double c = 3.0;
    return in / (1 - in.z / c);
}

void init_perspective(const double f) 
{
    Perspective = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, -1/f}}};
}

void init_viewport(const int x, const int y, const int w, const int h)
{
    Viewport = {{{w/2., 0, 0, x + w/2.}, {0, h/2., 0, y + h/2.}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
}

void init_zbuffer(const int width, const int height)
{
    zbuffer = std::vector(width * height, -1000.);
}





