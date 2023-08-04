#pragma once

#include <pixel.h>

double MaxScalarByPixels(const std::vector<Pixel>& pixels) {
    double max = 0;
    for (const Pixel& pixel : pixels) {
        Vector color = pixel.color;
        for (int i = 0; i < 3; ++i) {
            if (color[i] > max) {
                max = color[i];
            }
        }
    }
    return max;
}
double ToOneScale(double color_i, double max) {
    return (color_i + pow(color_i / max, 2)) / (color_i + 1);
}
Vector TransformColor(const Pixel& pixel, double max) {
    Vector result;
    const Vector& color = pixel.color;
    for (int i = 0; i < 3; ++i) {
        result[i] = pow(ToOneScale(color[i], max), 1 / 2.2);
    }
    return result;
}
void TransformAllColors(std::vector<Pixel>* pixels) {
    double max = MaxScalarByPixels(*pixels);
    for (Pixel& pixel : *pixels) {
        pixel.color = TransformColor(pixel, max);
    }
}

int ScalarToRGB(double c) {
    return round(c * 255);
}

void AllToRGB(std::vector<Pixel>* pixels) {
    for (Pixel& pixel : *pixels) {
        Vector color = pixel.color;
        pixel.rgb = {ScalarToRGB(color[0]), ScalarToRGB(color[1]), ScalarToRGB(color[2])};
    }
}

void FullToRGB(std::vector<Pixel>* pixels) {
    TransformAllColors(pixels);
    AllToRGB(pixels);
}

void DepthToRGB(std::vector<Pixel>* pixels) {
    double max = MaxScalarByPixels(*pixels);
    for (Pixel& pixel : *pixels) {
        Vector& color = pixel.color;
        if (color[0] < -0.5) {
            pixel.color = {1, 1, 1};
            continue;
        }
        for (int i = 0; i < 3; ++i) {
            color[i] /= max;
        }
    }
    AllToRGB(pixels);
}

void NormalToRGB(std::vector<Pixel>* pixels) {
    for (Pixel& pixel : *pixels) {
        Vector& color = pixel.color;
        for (int i = 0; i < 3; ++i) {
            color[i] = color[i] / 2 + 0.5;
        }
    }
    AllToRGB(pixels);
}