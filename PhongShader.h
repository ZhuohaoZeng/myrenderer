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
    vec4 varying_nrm[3];
    vec4 tri[3];

    PhongShader(const vec3 &light,  const model &m) : mo(m) {
        l = normalize(ModelView * vec4{light.x, light.y, light.z, 0.});
    }

    virtual vec4 vertex(const int face, const int vert) {
        varying_uv[vert]  = mo.uv(face, vert);
        varying_nrm[vert] = ModelView.invert_transpose() * mo.normal(face, vert);
        vec4 gl_Position = ModelView * mo.vert(face, vert);
        tri[vert] = gl_Position;
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {  
        mat<2,4> E = { tri[1]-tri[0], tri[2]-tri[0] };
        mat<2,2> U = { varying_uv[1]-varying_uv[0], varying_uv[2]-varying_uv[0] };
        mat<2,4> T = U.invert() * E;
        mat<4,4> D = {normalize(T[0]),  // tangent vector
                      normalize(T[1]),  // bitangent vector
                      normalize(varying_nrm[0]*bar[0] + varying_nrm[1]*bar[1] + varying_nrm[2]*bar[2]), // interpolated normal
                      {0,0,0,1}};
        //vec4 n = normalize(ModelView.invert_transpose() * mo.normal(uv));
        vec2 uv = bar[0] * varying_uv[0] + bar[1] * varying_uv[1] + bar[2] * varying_uv[2];
        vec4 n = normalize(D.transpose() * mo.normal(uv));
        vec4 r = normalize(2 * n * (n * l) - l);
        double ambient = .3;                                      // ambient light intensity
        double diff = std::max(0., n * l);
        double spec = (.5+2.*sample2D(mo.specular(), uv)[0]/255.) * std::pow(std::max(r.z, 0.), 35);
        TGAColor gl_FragColor = sample2D(mo.diffuse(), uv);
        for (int channel : {0,1,2})
            gl_FragColor[channel] = std::min<int>(255, gl_FragColor[channel]*(ambient + diff + spec));
        return {false, gl_FragColor};                                    // do not discard the pixel
    }
};