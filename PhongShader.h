#include <cmath>
#include "mygl.h"
#include "model.h"
#include "geometry.h"


extern mat<4,4> ModelView, Perspective; // "OpenGL" state matrices and
extern std::vector<double> zbuffer;

struct PhongShader : IShader {
    const model &mo;
    vec3 tri[3];  // triangle in eye coordinates
    vec3 l;

    PhongShader(const vec3 &light,  const model &m) : mo(m) {
        l = normalize((ModelView * vec4{light.x, light.y, light.z, 0.}).xyz());
    }

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = mo.vert(face, vert);                          // current vertex in object coordinates
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz();                            // in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        TGAColor gl_FragColor = {255, 255, 255, 255};  
        vec3 n = normalize(cross(tri[1] - tri[0], tri[2] - tri[0]));
        vec3 r = normalize(2 * n * (n * l) - l);
        double ambient = .3;                                      // ambient light intensity
        double diff = std::max(0., n * l);
        double spec = std::pow(std::max(r.z, 0.), 55);
        for (int channel : {0,1,2})
        {
            gl_FragColor[channel] *= std::min(1., ambient + .4*diff + .9*spec);
        }
        return {false, gl_FragColor};                                    // do not discard the pixel
    }
};