#include "Frame.h"

Frame::Frame(int width, int height) : width(width), height(height) {
    glyphs = new char[width * height];
    colors = new char[width * height];
    for (int i = 0; i < width * height; i++){
        glyphs[i] = 0;
        colors[i] = 0;
    }
}

void Frame::SetBit(int x, int y, char glyph, char col, bool transparent) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        char c = col;
        if (transparent)//fg only
            c = (colors[x + y * width] / 16) * 16 + col;
        glyphs[x + y * width] = glyph;
        colors[x + y * width] = c;
    }
}

void Frame::Fill(int x1, int y1, int x2, int y2, char glyph, char col) {
    if (x1 < 0)
        x1 = 0;
    else if (x1 > width)
        x1 = width;

    if (y1 < 0)
        y1 = 0;
    else if (y1 > height)
        y1 = height;

    if (x2 < 0)
        x2 = 0;
    else if (x2 > width)
        x2 = width;

    if (y2 < 0)
        y2 = 0;
    else if (y2 > height)
        y2 = height;

    for (int x = x1; x < x2; x++) {
        for (int y = y1; y < y2; y++)
            SetBit(x, y, glyph, col);
    }
}

void Frame::DrawString(int x, int y, std::string str, char col, bool transparent) {
    if (y < 0 || y >= height)
            return;

    for (unsigned int i = 0; i < str.size(); i++) {
        SetBit(x + i, y, str[i], col, transparent);
    }
}

void Frame::DrawBorder(char glyph, char col) {
    DrawRect(0, 0, width, height, glyph, col, false);
}

void Frame::DrawFrame(int x, int y, Frame* frame) {
    if (frame == nullptr)
        return;

    for (int i = 0; i < frame->width; i++) {
        for (int j = 0; j < frame->height; j++) {
            SetBit(x + i, y + j, frame->GetGlyph(i, j), frame->GetColor(i, j));
        }
    }
}

void Frame::DrawRect(int x, int y, int w, int h, char glyph, char col, bool transparent){
    for (int xx = x; xx < w + x; xx++) {
        SetBit(xx, 0, glyph, col);
        SetBit(xx, h - 1, glyph, col, transparent);
    }

    //verticals
    for (int yy = y; yy < h + y; yy++) {
        SetBit(0, yy, glyph , col);
        SetBit(w - 1, yy, glyph, col, transparent);
    }
}

Frame::~Frame() {
    delete[] glyphs;
    delete[] colors;
}