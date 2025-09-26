#include <cmath>
#include <ctime>
#include "tgaimage.h"
#include "model.h"
#include <iostream>

constexpr int width  = 800;
constexpr int height = 800;

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void drawLine(int startX, int startY, int endX, int endY, TGAImage &frameBuffer, const TGAColor &color)
{
    bool steep{};
    if (abs(endY - startY) > abs(endX - startX))//check if Y is more steep than X
    {
        steep = true;
        std::swap(startX, startY);
        std::swap(endY, endX);
    }
    if (endX < startX)
    {
        std::swap(endX, startX);
        std::swap(endY, startY);
    }
    float y = startY;
    int ierror = 0;
    for(float x = startX; x <= endX; ++x)
    {
        if (steep) frameBuffer.set(y, x, color);
        else frameBuffer.set(x, y, color);
        ierror += 2 * std::abs(endY - startY);
        y += (endY > startY ? 1 : -1) * (ierror > endX - startX);
        ierror -= 2 * (endX - startX) * (ierror > endX - startX);
    }
}

void drawTriangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    drawLine(ax, ay, bx, by, framebuffer, color);
    drawLine(bx, by, cx, cy, framebuffer, color);
    drawLine(cx, cy, ax, ay, framebuffer, color);
}

std::tuple<double, double> project(vec3 in)
{
    return {(in.x + 1.) * width  / 2,
            (in.y + 1.) * height / 2};
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }
    TGAImage framebuffer(width, height, TGAImage::RGB);
    model m{argv[1]};
    for (int i = 0; i < m.nfaces(); ++i)//draw triangle faces
    {
        auto [ax, ay] = project(m.vert(i, 0));
        auto [bx, by] = project(m.vert(i, 1));
        auto [cx, cy] = project(m.vert(i, 2));
        drawTriangle(ax, ay, bx, by, cx, cy, framebuffer, green);
    }
    for (int i = 0; i < m.nverts(); ++i)//draw vertices
    {
        auto [a, b] = project(m.vert(i));
        framebuffer.set(a, b, white);
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

