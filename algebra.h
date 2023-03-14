#pragma once

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

typedef struct Vector3i {
    int x;
    int y;
    int z;
} Vector3i;

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef struct Vector2i {
    int x;
    int y;
} Vector2i;

typedef struct Matrix4 {
    float data[16];
} Matrix4;

Vector3 vec3_add(Vector3* a, Vector3* b);
Vector3 vec3_sub(Vector3* a, Vector3* b);
Vector3 vec3_cross(Vector3* a, Vector3* b);
Vector3 vec3_norm(Vector3* a);
float vec3_dot(Vector3* a, Vector3* b);
Vector3 vec3_neg(Vector3* a);
Matrix4 camspace_matrix(Vector3* r, Vector3* u, Vector3* d, Vector3* e);
Matrix4 mat4_mul(Matrix4* a, Matrix4* b);
Vector3 to_camspace(Vector3* wc, Matrix4* csm);
Vector2 project(Vector3 point, float depth);
Vector3 project3(Vector3 point, float depth);
Vector2i to_raster_space(Vector2 point, int res_x, int res_y);
Vector3 raster_to_camspace(Vector2i* pixel, int res_x, int res_y);
