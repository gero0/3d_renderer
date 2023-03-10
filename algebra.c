#include "algebra.h"
#include <math.h>

Vector3 vec3_cross(Vector3* a, Vector3* b)
{
    Vector3 c = {
        (a->y * b->z) - (a->z * b->y),
        (a->z * b->x) - (a->x * b->z),
        (a->x * b->y) - (a->y * b->x),
    };
    return c;
}

Vector3 vec3_norm(Vector3* a)
{
    float len = sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
    Vector3 c = {
        a->x / len,
        a->y / len,
        a->z / len,
    };
    return c;
}

float vec3_dot(Vector3* a, Vector3* b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

Vector3 vec3_neg(Vector3* a)
{
    Vector3 c = {
        -a->x, -a->y, -a->z
    };
    return c;
}

Matrix4 camspace_matrix(Vector3* r, Vector3* u, Vector3* d, Vector3* e)
{
    Vector3 neg_e = vec3_neg(e);
    Vector3 neg_d = vec3_neg(d);

    Matrix4 mx = {
        r->x, r->y, r->z, vec3_dot(r, &neg_e),
        u->x, u->y, u->z, vec3_dot(u, &neg_e),
        neg_d.x, neg_d.y, neg_d.z, vec3_dot(d, e),
        0.0, 0.0, 0.0, 1.0
    };

    return mx;
}

Matrix4 mat4_mul(Matrix4* a, Matrix4* b)
{
    Matrix4 c;
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            float product = 0;

            for (int i = 0; i < 4; i++) {
                product += (a->data[y * 4 + i] * b->data[i * (4) + x]);
            }

            c.data[y * 4 + x] = product;
        }
    }

    return c;
}

Vector3 to_camspace(Vector3* wc, Matrix4* csm)
{
    Vector3 camera_coords = {
        csm->data[0] * wc->x + csm->data[1] * wc->y + csm->data[2] * wc->z + csm->data[3],
        csm->data[4] * wc->x + csm->data[5] * wc->y + csm->data[6] * wc->z + csm->data[7],
        csm->data[8] * wc->x + csm->data[9] * wc->y + csm->data[10] * wc->z + csm->data[11],
    };

    return camera_coords;
}
