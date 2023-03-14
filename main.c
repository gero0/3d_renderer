#include "algebra.h"
#include "line.h"
#include "triangle.h"
#include <MiniFB.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float fov_to_canvas_z(float fov_deg)
{
    float rad = fov_deg / 180.0 * M_PI;
    return 1.0 / tan(rad / 2);
}

void clear_screen(uint32_t pixels[], float* z_buffer, int res_x, int res_y)
{
    memset(pixels, 0, sizeof(uint32_t) * res_x * res_y);
    for (size_t i = 0; i < res_x * res_y; ++i) {
        z_buffer[i] = 999999.9;
    }
}

void render_point(Vector2 point, uint32_t pixels[], int res_x, int res_y)
{
    if (point.x <= 1.0 && point.x >= -1.0 && point.y <= 1.0 && point.y >= -1.0) {
        int x = (1 + point.x) / 2 * (res_x - 1);
        int y = (res_y - 1) - (1 + point.y) / 2 * (res_y - 1);
        pixels[y * res_x + x] = 0xFFFF00FF;
    }
}

bool visible(Vector3 point, float depth)
{
    return point.z > 0.1;
}

bool line_visible(Line3d line, float depth)
{
    return visible(line.a, depth) && visible(line.b, depth);
}

bool triangle_visible(Triangle* t, float depth)
{
    return visible(t->v1, depth) && visible(t->v2, depth) && visible(t->v3, depth);
}

int main(void)
{
    const int res_x = 640, res_y = 640;
    struct mfb_window* window = mfb_open("00_basic_window", res_x, res_y);

    uint32_t* pixels = (uint32_t*)malloc(sizeof(uint32_t) * res_x * res_y);
    float* z_buffer = (float*)malloc(sizeof(float) * res_x * res_y);

    clear_screen(pixels, z_buffer, res_x, res_y);

    Vector3 cam_pos = { 0.0, 0.0, 4.0 };
    Vector3 up_dir = { 0.0, 1.0, 0.0 };
    Vector3 look_dir = { 0.0, 0.0, 1.0 };

    float p = 0.0;
    float r = 4.0;

    Vector3 points[] = {
        { 1, -1, -1 },
        { 1, -1, 1 },
        { 1, 1, -1 },
        { 1, 1, 1 },
        { -1, -1, -1 },
        { -1, -1, 1 },
        { -1, 1, -1 },
        { -1, 1, 1 }
    };

    Line3d lines[] = {
        { points[0], points[1] },
        { points[0], points[2] },
        { points[0], points[4] },
        { points[1], points[5] },
        { points[1], points[3] },
        { points[4], points[5] },
        { points[4], points[6] },
        { points[2], points[3] },
        { points[2], points[6] },
        { points[7], points[5] },
        { points[7], points[3] },
        { points[7], points[6] },
    };

    Triangle triangles[] = {
        { { -1, -1, 1 }, { 0, 1, 0 }, { 1, -1, 1 }, { { 255, 0, 0 }, { 0, 255, 0 }, { 0, 0, 255 } } },
        { { 1, -1, 1 }, { 0, 1, 0 }, { 1, -1, -1 }, { { 0, 0, 0 }, { 255, 0, 0 }, { 0, 0, 0 } } },
        { { 1, -1, -1 }, { 0, 1, 0 }, { -1, -1, -1 }, { { 0, 255, 0 }, { 0, 255, 0 }, { 0, 255, 0 } } },
        { { -1, -1, -1 }, { 0, 1, 0 }, { -1, -1, 1 }, { { 0, 0, 255 }, { 0, 0, 255 }, { 0, 0, 255 } } },
        { { -1, -1, -1 }, { 0, 2, -1 }, { 1, -1, 1 }, { { 0, 0, 0 }, { 255, 255, 255 }, { 0, 0, 0 } } },
    };

    float fov
        = 90.0;
    float depth = fov_to_canvas_z(fov);

    do {
        // Vector3 cam_pos = { r * sin(p * M_PI), 0, r * cos(p * M_PI) };
        // Vector3 cam_pos = { 3 * sin(p * M_PI), 0, 4.0};
        // Vector3 cam_pos = { 0.0, 3 * sin(p * M_PI), 4.0 };
        Vector3 cam_pos = { r * sin(p * M_PI), 0, r * cos(p * M_PI) };
        // Vector3 cam_pos = { r * sin(p * M_PI), 3 * sin(p * M_PI), r * cos(p * M_PI) };
        Vector3 look_dir = { sin(p * M_PI), 0.0, cos(p * M_PI) };
        // Vector3 look_dir = { sin(p * M_PI), 0.0, 1.0 };
        // Vector3 look_dir = { 0.0, sin(p * M_PI), 1.0 };
        // Vector3 look_dir = { 0, 0.0, 1.0 };

        look_dir = vec3_norm(&look_dir);
        // cam_pos = vec3_norm(&cam_pos);

        Vector3 r = vec3_cross(&look_dir, &up_dir);
        Vector3 u = vec3_cross(&r, &look_dir);
        r = vec3_norm(&r);
        u = vec3_norm(&u);
        Matrix4 csm = camspace_matrix(&r, &u, &look_dir, &cam_pos);

        clear_screen(pixels, z_buffer, res_x, res_y);

        // for (int i = 0; i < sizeof(lines) / sizeof(Line3d); i++) {
        //     Line3d line = line_to_camspace(lines[i], csm);
        //     if (line_visible(line, depth)) {
        //         Line l = project_line(line, depth);
        //         render_line(l, pixels, res_x, res_y);
        //     }
        // }

        for (int i = 0; i < sizeof(triangles) / sizeof(Triangle); i++) {
            Triangle t = triangle_to_camspace(&triangles[i], &csm);
            t = project_triangle(&t, depth);
            if (triangle_visible(&t, depth)) {
                render_triangle(&t, pixels, z_buffer, res_x, res_y);
                // Vector3 normal = triangle_normal(&t);
                // Line3d nl = {{}, normal};
                // Line l = project_line(nl, depth);
                // printf("%f %f %f %f\n", l.a.x, l.a.y, l.b.x, l.b.y);
                // render_line(l, pixels, res_x, res_y);
            }
        }

        // for (int i = 0; i < sizeof(points) / sizeof(Vector3); i++) {
        //     Vector3 coords = to_camspace(&points[i], &csm);

        //     if (visible(coords, depth)) {
        //         Vector2 projected = project(coords, depth);
        //         render_point(projected, pixels, res_x, res_y);
        //     }
        // }

        p += 0.01;
        if (p > 2.0) {
            p = 0;
        }

        mfb_update_ex(window, pixels, res_x, res_y);
    }

    while (mfb_wait_sync(window));
}
