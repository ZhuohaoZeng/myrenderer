#include "mygl.h"
#include <algorithm>
#include <limits>

mat<4,4> ModelView, Viewport, Perspective;
std::vector<double> zbuffer;

void lookat(const vec3 eye, const vec3 center, const vec3 up)
{
    vec3 n = normalize(eye - center);
    vec3 l = normalize(cross(up, n));
    vec3 m = normalize(cross(n, l));
    ModelView = mat<4,4>{{{l.x, l.y, l.z, 0}, {m.x, m.y, m.z, 0}, {n.x, n.y, n.z, 0}, {0, 0, 0, 1}}} 
    * mat<4,4>{{{1, 0, 0, -center.x}, {0, 1, 0, -center.y}, {0, 0, 1, -center.z}, {0, 0, 0, 1}}};
}

void initPerspective(double f) 
{
    Perspective = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, -1/f, 1}}};
}


void initViewport(const int x, const int y, const int w, const int h)
{
    Viewport = {{{w/2., 0,    0, x + w/2.},
                 {0,    h/2., 0, y + h/2.},
                 {0,    0,    1, 0       },
                 {0,    0,    0, 1       }}};
}

void initZbuffer(int w, int h)
{
    zbuffer = std::vector(w*h, -std::numeric_limits<double>::max());
}

void rasterize(const Triangle &clip, const IShader &shader, TGAImage &framebuffer)
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
            auto [discard, color] = shader.fragment(bc); // let shader decided draw or not and its color based on the barcentric of current fragment
            zbuffer[x + framebuffer.width() * y] = z;
            framebuffer.set(x, y, color);
        }
    }

}




