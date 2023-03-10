#pragma once

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

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

Vector3 vec3_cross(Vector3* a, Vector3* b);
Vector3 vec3_norm(Vector3* a);
float vec3_dot(Vector3* a, Vector3* b);
Vector3 vec3_neg(Vector3* a);
Matrix4 camspace_matrix(Vector3* r, Vector3* u, Vector3* d, Vector3* e);
Matrix4 mat4_mul(Matrix4* a, Matrix4* b);
Vector3 to_camspace(Vector3* wc, Matrix4* csm);
