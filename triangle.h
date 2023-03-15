#pragma once
#include "algebra.h"
#include "stdint.h"

#define ColorWhite {255, 255, 255}
#define ColorRed {255, 0, 0}
#define ColorGreen {0, 255, 0}
#define ColorBlue {0, 0, 255}

typedef struct RgbColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RgbColor;

typedef struct Triangle {
    Vector3 v1;
    Vector3 v2;
    Vector3 v3;
    RgbColor colors[3];
} Triangle;

typedef struct BBox {
    Vector2i topleft;
    Vector2i bottomright;
} BBox;

Triangle
triangle_to_camspace(Triangle* t, Matrix4* csm);
Triangle project_triangle(Triangle* t, float depth);
BBox triangle_get_bbox(Vector2i* v1, Vector2i* v2, Vector2i* v3);
void render_triangle(Triangle* t, uint32_t pixels[], float z_buffer[], int res_x, int res_y);
Vector3 triangle_normal(Triangle* t);
Vector3 triangle_midpoint(Triangle* t);
