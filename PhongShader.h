#include <cmath>
#include "mygl.h"
#include "model.h"
#include "geometry.h"


extern mat<4,4> ModelView, Perspective; // "OpenGL" state matrices and
extern std::vector<double> zbuffer;

struct PhongShader : IShader {
    const model &mo;
    vec4 l;
    vec2 varying_uv[3];

    PhongShader(const vec3 &light,  const model &m) : mo(m) {
        l = normalize(ModelView * vec4{light.x, light.y, light.z, 0.});
    }

    virtual vec4 vertex(const int face, const int vert) {
        varying_uv[vert] = mo.uv(face, vert);
        vec4 gl_Position = ModelView * mo.vert(face, vert);
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        TGAColor gl_FragColor = {255, 255, 255, 255};  
        vec2 uv = bar[0] * varying_uv[0] + bar[1] * varying_uv[1] + bar[2] * varying_uv[2];
        vec4 n = normalize(ModelView.invert_transpose() * mo.normal(uv));
        vec4 r = normalize(2 * n * (n * l) - l);
        double ambient = .3;                                      // ambient light intensity
        double diff = std::max(0., n * l);
        double spec = std::pow(std::max(r.z, 0.), 55);
        for (int channel : {0,1,2})
            gl_FragColor[channel] *= std::min(1., ambient + .4*diff + .9*spec);
        return {false, gl_FragColor};                                    // do not discard the pixel
    }
};