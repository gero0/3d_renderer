#pragma once
#include "algebra.h"
#include "stdint.h"

typedef struct RgbColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RgbColor;

//params: barycentric coords and data struct (shader-dependent)
//returns: rgbcolor of pixel;
typedef RgbColor (*FragmentShader)(Vector3, void*);

typedef struct Triangle {
    Vector3 v1;
    Vector3 v2;
    Vector3 v3;
    RgbColor colors[3];
    // FragmentShader shader;
    // void* shader_data;
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
