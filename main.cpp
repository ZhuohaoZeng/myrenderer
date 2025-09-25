#include <cmath>
#include "tgaimage.h"
#include <ctime>

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
        //int midX = std::round(startX + t * (endX  - startX));
        if (steep) frameBuffer.set(y, x, color);
        else frameBuffer.set(x, y, color);
        ierror += 2 * std::abs(endY - startY);
        y += (endY > startY ? 1 : -1) * (ierror > endY - startY);
        ierror -= 2 * (endY - startY) * (ierror > endY - startY);
    }
}

int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    // framebuffer.set(ax, ay, white);
    // drawLine(ax, ay, bx, by, framebuffer, blue);
    // framebuffer.set(bx, by, white);
    // drawLine(cx, cy, bx, by,  framebuffer, green);
    // framebuffer.set(cx, cy, white);
    // drawLine(cx, cy, ax, ay, framebuffer, yellow);
    // drawLine(ax, ay, cx, cy, framebuffer, red);
    std::srand(std::time({}));
    for (int i=0; i<(1<<24); i++) {
        int ax = rand()%width, ay = rand()%height;
        int bx = rand()%width, by = rand()%height;
        drawLine(ax, ay, bx, by, framebuffer, {
            static_cast<std::uint8_t>(rand() % 255),
            static_cast<std::uint8_t>(rand() % 255),
            static_cast<std::uint8_t>(rand() % 255),
            static_cast<std::uint8_t>(rand() % 255)
        });
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

