#include "triangle.h"
#include "algebra.h"
#include <stdbool.h>

static int min(int a, int b)
{
    return a < b ? a : b;
}

static int max(int a, int b)
{
    return a > b ? a : b;
}

Vector3 barycentric_coords(Vector2i* p, Vector2i* v1, Vector2i* v2, Vector2i* v3)
{
    float w1 = (float)((v2->y - v3->y) * (p->x - v3->x) + (v3->x - v2->x) * (p->y - v3->y))
        / ((v2->y - v3->y) * (v1->x - v3->x) + (v3->x - v2->x) * (v1->y - v3->y));

    float w2 = (float)((v3->y - v1->y) * (p->x - v3->x) + (v1->x - v3->x) * (p->y - v3->y))
        / ((v2->y - v3->y) * (v1->x - v3->x) + (v3->x - v2->x) * (v1->y - v3->y));

    float w3 = 1 - w1 - w2;

    Vector3 bary = { w1, w2, w3 };

    return bary;
}

bool triangle_edge_function(Vector2i* a, Vector2i* b, Vector2i* c)
{
    return ((c->x - a->x) * (b->y - a->y) - (c->y - a->y) * (b->x - a->x) >= 0);
}

Triangle triangle_to_camspace(Triangle* t, Matrix4* csm)
{
    Triangle ct = {
        to_camspace(&t->v1, csm),
        to_camspace(&t->v2, csm),
        to_camspace(&t->v3, csm),
        {
            t->colors[0],
            t->colors[1],
            t->colors[2],
        }
    };
    return ct;
}

Triangle project_triangle(Triangle* t, float depth)
{
    Triangle projected = {
        project3(t->v1, depth),
        project3(t->v2, depth),
        project3(t->v3, depth),
        {
            t->colors[0],
            t->colors[1],
            t->colors[2],
        }
    };
    return projected;
}

BBox triangle_get_bbox(Vector2i* v1, Vector2i* v2, Vector2i* v3)
{
    int min_x = min(v1->x, min(v2->x, v3->x));
    int min_y = min(v1->y, min(v2->y, v3->y));
    int max_x = max(v1->x, max(v2->x, v3->x));
    int max_y = max(v1->y, max(v2->y, v3->y));
    BBox box = { { min_x, min_y }, { max_x, max_y } };
    return box;
}

uint32_t calculate_color(Triangle* t, Vector3* bary)
{
    uint32_t avg_red = bary->x * t->colors[0].r + bary->y * t->colors[1].r + bary->z * t->colors[2].r;
    uint32_t avg_green = bary->x * t->colors[0].g + bary->y * t->colors[1].g + bary->z * t->colors[2].g;
    uint32_t avg_blue = bary->x * t->colors[0].b + bary->y * t->colors[1].b + bary->z * t->colors[2].b;

    uint32_t color = avg_blue | (avg_green << 8) | (avg_red << 16);
    return color;
}

void render_triangle(Triangle* t, uint32_t pixels[], float z_buffer[], int res_x, int res_y)
{
    Vector2i v1 = to_raster_space((Vector2) { t->v1.x, t->v1.y }, res_x, res_y);
    Vector2i v2 = to_raster_space((Vector2) { t->v2.x, t->v2.y }, res_x, res_y);
    Vector2i v3 = to_raster_space((Vector2) { t->v3.x, t->v3.y }, res_x, res_y);

    BBox bbox = triangle_get_bbox(&v1, &v2, &v3);

    for (int y = bbox.topleft.y; y < bbox.bottomright.y; y++) {
        for (int x = bbox.topleft.x; x < bbox.bottomright.x; x++) {
            if (x >= 0 && x <= res_x - 1 && y >= 0 && y <= res_y - 1) {
                Vector2i p = { x, y };
                Vector3 bary = barycentric_coords(&p, &v1, &v2, &v3);
                bool inside = bary.x >= 0 && bary.y >= 0 && bary.z >= 0;

                if (inside) {
                    float depth = 1 / (bary.x * (1 / t->v1.z) + bary.y * (1 / t->v2.z) + bary.z * (1 / t->v3.z));
                    if (depth < z_buffer[y * res_x + x]) {
                        pixels[y * res_x + x] = calculate_color(t, &bary);
                        z_buffer[y * res_x + x] = depth;
                    }
                }
            }
        }
    }
}

Vector3 triangle_normal(Triangle* t)
{
    Vector3 v = vec3_sub(&t->v2, &t->v1);
    Vector3 w = vec3_sub(&t->v3, &t->v1);
    Vector3 cross = vec3_cross(&v, &w);
    return vec3_norm(&cross);
}

Vector3 triangle_midpoint(Triangle* t)
{
    return (Vector3) {
        (t->v1.x + t->v2.x + t->v3.x) / 3.0,
        (t->v1.y + t->v2.y + t->v3.y) / 3.0,
        (t->v1.z + t->v2.z + t->v3.z) / 3.0,
    };
}
