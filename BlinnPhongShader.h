#include <cmath>
#include "mygl.h"
#include "model.h"
#include "geometry.h"


extern mat<4,4> ModelView, Perspective; // "OpenGL" state matrices and
extern std::vector<double> zbuffer;

struct BlinnPhongShader : IShader {
    const model &mo;
    vec3 tri[3];  // triangle in eye coordinates
    vec3 l;
    vec3 varying_nrm[3];

    BlinnPhongShader(const vec3 &light, const vec3 &eye,  const model &m) : mo(m) {
        l = normalize((ModelView * vec4{light.x, light.y, light.z, 0.}).xyz());
    }

    // virtual vec4 vertex(const int face, const int vert) {
    //     vec3 curVertex = mo.vert(face, vert);                          // current vertex in object coordinates
    //     vec4 gl_Position = ModelView * vec4{curVertex.x, curVertex.y, curVertex.z, 1.};
    //     vec3 norm = mo.normal(face, vert);                     // current vertex in object coordinates
    //     varying_nrm[vert] = (ModelView.invert_transpose() * vec4{norm.x, norm.y, norm.z, 0.}).xyz();
    //     tri[vert] = gl_Position.xyz();                            // in eye coordinates
    //     return Perspective * gl_Position;                         // in clip coordinates
    // }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        TGAColor gl_FragColor = {255, 255, 255, 255};
        vec3 v = vec3{0, 0, 1};
        vec3 n = normalize(varying_nrm[0]*bar.x + varying_nrm[1]*bar.y + varying_nrm[2]*bar.z);
        vec3 h = normalize(v + l);
        double ambient = .3;                                      // ambient light intensity
        double diff = std::max(0., n * l);
        double spec = std::pow(std::max(n * h, 0.), 200);
        for (int channel : {0,1,2})
        {
            gl_FragColor[channel] *= std::min(1., ambient + .4*diff + .9*spec);
        }
        return {false, gl_FragColor};                                    // do not discard the pixel
    }
};