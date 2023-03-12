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
        t->color
    };
    return ct;
}

Triangle project_triangle(Triangle* t, float depth)
{
    Triangle projected = {
        project3(t->v1, depth),
        project3(t->v2, depth),
        project3(t->v3, depth),
        t->color
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

void render_triangle(Triangle* t, uint32_t pixels[], int res_x, int res_y)
{
    Vector2i v1 = to_raster_space((Vector2) { t->v1.x, t->v1.y }, res_x, res_y);
    Vector2i v2 = to_raster_space((Vector2) { t->v2.x, t->v2.y }, res_x, res_y);
    Vector2i v3 = to_raster_space((Vector2) { t->v3.x, t->v3.y }, res_x, res_y);

    BBox bbox = triangle_get_bbox(&v1, &v2, &v3);

    for (int y = bbox.topleft.y; y < bbox.bottomright.y; y++) {
        for (int x = bbox.topleft.x; x < bbox.bottomright.x; x++) {
            //TODO: depth calculation and z-buffer
            if (x > 0 && x < res_x - 1 && y > 0 && y < res_y - 1) {
                bool inside = true;
                Vector2i p = {x, y};
                inside &= triangle_edge_function(&v1, &v2, &p);
                inside &= triangle_edge_function(&v2, &v3, &p);
                inside &= triangle_edge_function(&v3, &v1, &p);

                if (inside) {
                    pixels[y * res_x + x] = t->color;
                }
            }
        }
    }
}
