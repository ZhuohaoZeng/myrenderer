#include "tgaimage.h"
#include "geometry.h"

#pragma once
#ifndef MYGL_H_
#define MYGL_H_

void lookat(const vec3 eye, const vec3 center, const vec3 up);
void initPerspective(const double f);
void initViewport(const int x, const int y, const int w, const int h);
void initZbuffer(const int width, const int height);

struct IShader {
    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const = 0;
};

typedef vec4 Triangle[3]; // a triangle primitive is made of three ordered points
void rasterize(const Triangle &clip, const IShader &shader, TGAImage &framebuffer);

#endif