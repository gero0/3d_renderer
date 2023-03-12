#include "line.h"

Line project_line(Line3d line, float depth)
{
    Line l = { project(line.a, depth), project(line.b, depth) };
    return l;
}

Line3d line_to_camspace(Line3d line, Matrix4 csm)
{
    Line3d l = {
        to_camspace(&line.a, &csm),
        to_camspace(&line.b, &csm),
    };

    return l;
}

void render_line(Line line, uint32_t pixels[], int res_x, int res_y)
{
    Vector2i a = to_raster_space(line.a, res_x, res_y);
    Vector2i b = to_raster_space(line.b, res_x, res_y);

    if (a.x == b.x) {
        int min_y = a.y < b.y ? a.y : b.y;
        int max_y = a.y > b.y ? a.y : b.y;

        if (a.x > 0 && a.x < res_x - 1) {
            min_y = min_y > 0 ? min_y : 0;
            max_y = max_y < (res_y - 1) ? max_y : (res_y - 1);
            for (int i = min_y; i < max_y; i++) {
                pixels[i * res_x + a.x] = 0xFFFFFFFF;
            }
        }

        return;
    }

    if (a.x > b.x) {
        Vector2i temp = a;
        a = b;
        b = temp;
    }

    for (int i = a.x; i < b.x; i++) {
        float ratio = (float)(i - a.x) / (b.x - a.x);
        int y = a.y + ratio * (b.y - a.y);
        if (i > 0 && i < res_x - 1 && y > 0 && y < res_y - 1) {
            pixels[y * res_x + i] = 0xFFFFFFFF;
        }
    }
}

