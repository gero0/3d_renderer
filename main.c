#include "algebra.h"
#include <MiniFB.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line3d {
    Vector3 a;
    Vector3 b;
} Line3d;

typedef struct Line {
    Vector2 a;
    Vector2 b;
} Line;

typedef struct Triangle {
    Vector3 v1;
    Vector3 v2;
    Vector3 v3;
    uint32_t color;
} Triangle;

float fov_to_canvas_z(float fov_deg)
{
    float rad = fov_deg / 180.0 * M_PI;
    return 1.0 / tan(rad / 2);
}

Vector2 project(Vector3 point, float depth)
{
    Vector2 projected_point = { point.x * depth / point.z, point.y * depth / point.z };
    return projected_point;
}

Vector3 project3(Vector3 point, float depth)
{
    Vector3 projected_point = { point.x * depth / point.z, point.y * depth / point.z, point.z };
    return projected_point;
}

void clear_screen(uint32_t pixels[], int res_x, int res_y)
{
    memset(pixels, 0, sizeof(uint32_t) * res_x * res_y);
}

Vector2i to_raster_space(Vector2 point, int res_x, int res_y)
{
    int x = (1 + point.x) / 2 * (res_x - 1);
    int y = (res_y - 1) - (1 + point.y) / 2 * (res_y - 1);
    Vector2i v = { x, y };
    return v;
}

void render_point(Vector2 point, uint32_t pixels[], int res_x, int res_y)
{
    if (point.x <= 1.0 && point.x >= -1.0 && point.y <= 1.0 && point.y >= -1.0) {
        int x = (1 + point.x) / 2 * (res_x - 1);
        int y = (res_y - 1) - (1 + point.y) / 2 * (res_y - 1);
        pixels[y * res_x + x] = 0xFFFF00FF;
    }
}

// Line truncate_line(Line line)
// {
//     Line org_line = line;

//     if (line.a.x < -1.0)
//         line.a.x = -1.0;

//     if (line.a.x > 1.0)
//         line.a.x = 1.0;

//     if (line.a.y < -1.0)
//         line.a.y = -1.0;

//     if (line.a.y > 1.0)
//         line.a.y = 1.0;

//     if (line.b.x < -1.0)
//         line.b.x = -1.0;

//     if (line.b.x > 1.0)
//         line.b.x = 1.0;

//     if (line.b.y < -1.0)
//         line.b.y = -1.0;

//     if (line.b.y > 1.0)
//         line.b.y = 1.0;

//     if (line.a.x == line.b.x || line.a.y == line.b.y) {
//         return org_line;
//     }

//     return line;
// }

Triangle triangle_to_camspace(Triangle* t, Matrix4* csm)
{
    Triangle ct = { to_camspace(&t->v1, csm), to_camspace(&t->v2, csm), to_camspace(&t->v3, csm), t->color };
    return ct;
}

Triangle project_triangle(Triangle* t, float depth)
{
    Triangle projected = { project3(t->v1, depth), project3(t->v2, depth), project3(t->v3, depth), t->color };
    return projected;
}

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

bool visible(Vector3 point, float depth)
{
    return point.z > depth;
}

bool line_visible(Line3d line, float depth)
{
    return visible(line.a, depth) && visible(line.b, depth);
}

int main(void)
{
    const int res_x = 640, res_y = 640;
    struct mfb_window* window = mfb_open("00_basic_window", res_x, res_y);

    uint32_t* pixels = (uint32_t*)malloc(sizeof(uint32_t) * res_x * res_y);
    memset(pixels, 0, sizeof(uint32_t) * res_x * res_y);

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

    float fov = 90.0;
    float depth = fov_to_canvas_z(fov);

    do {
        // Vector3 cam_pos = { r * sin(p * M_PI), 0, r * cos(p * M_PI) };
        // Vector3 cam_pos = { 3 * sin(p * M_PI), 0, 4.0};
        // Vector3 cam_pos = { 0.0, 3 * sin(p * M_PI), 4.0 };
        Vector3 cam_pos = { r * sin(p * M_PI), 3 * sin(p * M_PI), r * cos(p * M_PI) };
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
        Matrix4 t = camspace_matrix(&r, &u, &look_dir, &cam_pos);

        clear_screen(pixels, res_x, res_y);

        for (int i = 0; i < sizeof(lines) / sizeof(Line3d); i++) {
            Line3d line = line_to_camspace(lines[i], t);
            if (line_visible(line, depth)) {
                Line l = project_line(line, depth);
                render_line(l, pixels, res_x, res_y);
            }
        }

        for (int i = 0; i < sizeof(points) / sizeof(Vector3); i++) {
            Vector3 coords = to_camspace(&points[i], &t);

            if (visible(coords, depth)) {
                Vector2 projected = project(coords, depth);
                render_point(projected, pixels, res_x, res_y);
            }
        }

        p += 0.01;
        if (p > 2.0) {
            p = 0;
        }

        mfb_update_ex(window, pixels, res_x, res_y);
    }

    while (mfb_wait_sync(window));
}
