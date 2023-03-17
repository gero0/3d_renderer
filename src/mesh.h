#pragma once
#include "triangle.h"
#include <stdint.h>

typedef struct Mesh {
    char* name;
    Triangle* triangles;
    uint32_t triangle_count;
    Vector3 origin;
} Mesh;
