#pragma once
#include "algebra.h"
#include "stdint.h"

typedef struct Triangle {
    Vector3 v1;
    Vector3 v2;
    Vector3 v3;
    uint32_t color;
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
