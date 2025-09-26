#include <cmath>
#include <ctime>
#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <algorithm>

constexpr int width  = 120;
constexpr int height = 120;

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
    if (ay>by) { std::swap(ax, bx); std::swap(ay, by); }
    if (ay>cy) { std::swap(ax, cx); std::swap(ay, cy); }
    if (by>cy) { std::swap(bx, cx); std::swap(by, cy); }
    int totalHeight = cy - ay;
    if (ay != by)// avoid situation where two line overlapped
    {
        int halfHeight = by - ay;
        for (int y = ay; y <= by; ++y)
        {
            int x1 = ax + ( (cx - ax) * (y - ay) ) / totalHeight;//This calculate the point on the line from ax to cx but only until the height of bx
            int x2 = ax + ( (bx - ax) * (y - ay) ) / halfHeight;//This calculate the point on the line from ax to bx
            if(x1 > x2) std::swap(x1, x2); // ensucre x1 is the left bound and x2 is the y bound
            for (int i = x1; i <= x2; ++i) framebuffer.set(i, y, color);
        }
    }
    if (cy != by)
    {
        int halfHeight = cy - by;
        for (int y = by; y <= cy; ++y)
        {
            int x1 = ax + ( (cx - ax) * (y - ay) ) / totalHeight;//This calculate the point on the line from ax to cx but start at the height of bx
            int x2 = bx + ( (cx - bx) * (y - by) ) / halfHeight;//This calculate the point on the line from ax to bx
            if(x1 > x2) std::swap(x1, x2); // ensucre x1 is the left bound and x2 is the y bound
            for (int i = x1; i <= x2; ++i) framebuffer.set(i, y, color);
        }
    }
}


int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);
    drawTriangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    drawTriangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    drawTriangle(115, 83, 80,  90, 85, 120, framebuffer, blue);
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

