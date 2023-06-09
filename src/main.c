#include "algebra.h"
#include "line.h"
#include "mesh.h"
#include "obj_parser.h"
#include "triangle.h"
#include <MiniFB.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Vector3 cam_pos = { 0.0, 3.0, 6.0 };
static Vector3 up_dir = { 0.0, 1.0, 0.0 };
static Vector3 look_dir = { 0.0, 0.0, -1.0 };

float pitch = 0.5;
float yaw = 0.25;

bool render_normals = false;
float normal_scale = 0.1;

void update_look_dir()
{
    if (pitch > 1.0) {
        pitch = 0.0;
    }

    if (pitch < 0.0) {
        pitch = 1.0;
    }

    if (yaw > 1.0) {
        yaw = 0.0;
    }

    if (yaw < 0.0) {
        yaw = 1.0;
    }

    look_dir.x = cos(yaw * 2 * M_PI) * cos(pitch * 2 * M_PI);
    look_dir.y = sin(pitch * 2 * M_PI);
    look_dir.z = sin(yaw * 2 * M_PI) * cos(pitch * 2 * M_PI);
    look_dir = vec3_norm(&look_dir);
    printf("PITCH: %f, YAW: %f, LOOK DIR: %f %f %f\n", pitch, yaw, look_dir.x, look_dir.y, look_dir.z);
}

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
        pixels[y * res_x + x] = 0xFFFFFFFF;
    }
}

bool visible(Vector3 point, float depth)
{
    return point.z > 1.0;
    // return true;
}

bool line_visible(Line3d line, float depth)
{
    return visible(line.a, depth) && visible(line.b, depth);
}

bool triangle_visible(Triangle* t, float depth, Vector3 normal)
{
    if (normal.z < 0) {
        return false;
    }
    return visible(t->v1, depth) && visible(t->v2, depth) && visible(t->v3, depth);
}

Line3d get_triangle_normal(Triangle* t, float scale)
{
    Vector3 midpoint = triangle_midpoint(t);
    Vector3 normal = triangle_normal(t);
    Vector3 s_normal = vec3_scale(&normal, scale);
    return (Line3d) { midpoint, vec3_add(&midpoint, &s_normal) };
}

void keyboard(struct mfb_window* window, mfb_key key, mfb_key_mod mod, bool isPressed)
{
    float speed = 0.5;
    float look_speed = 0.05;

    if (!isPressed) {
        return;
    }

    if (key == KB_KEY_S) {
        Vector3 move_vec = vec3_scale(&look_dir, speed);
        cam_pos = vec3_sub(&cam_pos, &move_vec);
    } else if (key == KB_KEY_W) {
        Vector3 move_vec = vec3_scale(&look_dir, speed);
        cam_pos = vec3_add(&cam_pos, &move_vec);
    } else if (key == KB_KEY_D) {
        Vector3 right_vec = vec3_cross(&look_dir, &up_dir);
        right_vec = vec3_norm(&right_vec);
        right_vec = vec3_scale(&right_vec, speed);
        cam_pos = vec3_add(&cam_pos, &right_vec);
    } else if (key == KB_KEY_A) {
        Vector3 right_vec = vec3_cross(&look_dir, &up_dir);
        right_vec = vec3_norm(&right_vec);
        right_vec = vec3_scale(&right_vec, speed);
        cam_pos = vec3_sub(&cam_pos, &right_vec);
    } else if (key == KB_KEY_LEFT_SHIFT) {
        cam_pos.y += speed;
    } else if (key == KB_KEY_LEFT_CONTROL) {
        cam_pos.y -= speed;
    } else if (key == KB_KEY_LEFT) {
        yaw -= look_speed;
        update_look_dir();
    } else if (key == KB_KEY_RIGHT) {
        yaw += look_speed;
        update_look_dir();
    } else if (key == KB_KEY_UP) {
        pitch += look_speed;
        update_look_dir();
    } else if (key == KB_KEY_DOWN) {
        pitch -= look_speed;
        update_look_dir();
    } else if (key == KB_KEY_N) {
        render_normals = !render_normals;
    }
}

// Use wisely this event. It can be sent too often
void mouse_move(struct mfb_window* window, int x, int y)
{
    printf("%d %d\n", x, y);
}

int main(void)
{
    Mesh mesh;
    parse_obj_file("/home/gero/models/shrek.obj", &mesh);
    const int res_x = 640, res_y = 640;
    struct mfb_window* window = mfb_open("00_basic_window", res_x, res_y);

    uint32_t* pixels = (uint32_t*)malloc(sizeof(uint32_t) * res_x * res_y);
    float* z_buffer = (float*)malloc(sizeof(float) * res_x * res_y);

    clear_screen(pixels, z_buffer, res_x, res_y);

    mfb_set_mouse_move_callback(window, mouse_move);
    mfb_set_keyboard_callback(window, keyboard);

    float p = 0.0;
    float r = 6.0;

    Triangle* triangles = mesh.triangles;

    float fov
        = 90.0;
    float depth = fov_to_canvas_z(fov);

    do {
        // Vector3 cam_pos = { r * sin(p * M_PI), 3, r * cos(p * M_PI) };
        // Vector3 look_dir = { -sin(p * M_PI), 0.0, -cos(p * M_PI) };

        look_dir = vec3_norm(&look_dir);

        Vector3 r = vec3_cross(&look_dir, &up_dir);
        Vector3 u = vec3_cross(&r, &look_dir);
        r = vec3_norm(&r);
        u = vec3_norm(&u);
        Matrix4 csm = camspace_matrix(&r, &u, &look_dir, &cam_pos);

        clear_screen(pixels, z_buffer, res_x, res_y);

        for (int i = 0; i < mesh.triangle_count; i++) {
            Triangle t = triangle_to_camspace(&triangles[i], &csm);
            Line3d normal_line = get_triangle_normal(&t, normal_scale);
            t = project_triangle(&t, depth);
            Vector3 proj_norm = triangle_normal(&t);
            if (triangle_visible(&t, depth, proj_norm)) {
                render_triangle(&t, pixels, z_buffer, res_x, res_y);
                normal_line = project_line(normal_line, depth);
                if (line_visible(normal_line, depth) && render_normals) {
                    render_line(normal_line, pixels, res_x, res_y);
                }
            }
        }

        // printf("X:%f Y:%f Z:%f\n", cam_pos.x, cam_pos.y, cam_pos.z);

        p += 0.01;
        if (p > 2.0) {
            p = 0;
        }

        mfb_update_ex(window, pixels, res_x, res_y);
    }

    while (mfb_wait_sync(window));
}
