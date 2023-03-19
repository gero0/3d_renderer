#pragma once
#include "algebra.h"
#include <stdint.h>

typedef struct Line3d {
    Vector3 a;
    Vector3 b;
} Line3d;

typedef struct Line {
    Vector2 a;
    Vector2 b;
} Line;

Line3d project_line(Line3d line, float depth);
Line3d line_to_camspace(Line3d line, Matrix4 csm);
void render_line(Line3d line, uint32_t pixels[], int res_x, int res_y);
