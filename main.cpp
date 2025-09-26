#include <cmath>
#include <ctime>
#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <algorithm>

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

double signedTriangleArea(int ax, int ay, int bx, int by, int cx, int cy) {
    return ax * (by -cy) + bx * (cy - ay) + cx * (ay - by);
}

void drawTriangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color)
{
    int left = std::min(std::min(ax, bx), cx);
    int right = std::max(std::max(ax, bx), cx);
    int bottom = std::min(std::min(ay, by), cy);
    int top = std::max(std::max(ay, by), cy);
    double sarea = signedTriangleArea(ax, ay, bx, by, cx, cy);
    if (sarea<1) return;
    for (int x = left; x <= right; ++x)
    {
        for (int y = bottom; y <= top; ++y)
        {
            //using baycentric coordinate to check if this point is in the triangle.
            double l1 = signedTriangleArea(x, y, bx, by, cx, cy) / sarea;
            double l2 = signedTriangleArea(ax, ay, x, y, cx, cy) / sarea;
            double l3 = signedTriangleArea(ax, ay, bx, by, x, y) / sarea;
            if (l1 < 0 || l2 < 0 || l3 < 0) continue;
            framebuffer.set(x, y, color);
        }
    }
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
    model model{argv[1]};
    TGAImage framebuffer(width, height, TGAImage::RGB);
    for (int i=0; i<model.nfaces(); i++) { // iterate through all triangles
        auto [ax, ay] = project(model.vert(i, 0));
        auto [bx, by] = project(model.vert(i, 1));
        auto [cx, cy] = project(model.vert(i, 2));
        TGAColor rnd;
        for (int c=0; c<3; c++) rnd[c] = std::rand()%255;
        drawTriangle(ax, ay, bx, by, cx, cy, framebuffer, rnd);
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

