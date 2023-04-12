#include "algebra.h"
#include "mesh.h"
#include "triangle.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool process_vertex(const char* line_buf, Vector3* v)
{
    int result = sscanf(line_buf, "%*s %f %f %f", &v->x, &v->y, &v->z);
    return result == 3;
}

bool process_face(const char* line_buf, uint32_t* vert_ids)
{
    char tokens[3][32];
    int read = sscanf(line_buf, "%*s %s %s %s", tokens[0], tokens[1], tokens[2]);

    if (read < 3) {
        return false;
    }

    for (int i = 0; i < 3; i++) {
        int res = sscanf(tokens[i], "%u/%*u/%*u", &vert_ids[i]);
        if (res < 1) {
            return false;
        }
    }

    return true;
}

int parse_obj_file(const char* path, Mesh* mesh)
{
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        goto error;
    }

    char line_buf[256];
    uint32_t triangle_count = 0;
    uint32_t vertex_count = 0;

    //count triangles and vertices
    while (fgets(line_buf, sizeof(line_buf), file)) {
        if (line_buf[0] == 'f') {
            triangle_count += 1;
        } else if (line_buf[0] == 'v' && line_buf[1] == ' ') {
            vertex_count += 1;
        }
    }

    //load vertices
    fseek(file, 0, SEEK_SET);
    uint32_t v_i = 0;
    Vector3* verts = malloc(sizeof(Vector3) * vertex_count);

    while (fgets(line_buf, sizeof(line_buf), file)) {
        if (line_buf[0] == 'v' && line_buf[1] == ' ') {
            Vector3 v;
            int result = process_vertex(line_buf, &v);
            if (!result) {
                puts("Error while parsing vertex");
                goto error;
            }
            verts[v_i] = v;
            v_i++;
        }
    }

    //load triangles
    fseek(file, 0, SEEK_SET);
    Triangle* triangles = malloc(sizeof(Triangle) * triangle_count);
    uint32_t t_i = 0;
    while (fgets(line_buf, sizeof(line_buf), file)) {
        if (line_buf[0] == 'f') {
            uint32_t vert_ids[3];
            int result = process_face(line_buf, vert_ids);
            if (!result) {
                puts("Error while parsing face");
                goto error;
            }

            Triangle t = { verts[vert_ids[0] - 1], verts[vert_ids[1] - 1], verts[vert_ids[2] - 1], { ColorGreen, ColorRed, ColorBlue } };
            triangles[t_i] = t;
            t_i++;
        }
    }

    mesh->triangle_count = t_i;
    mesh->triangles = triangles;

    return 0;

error:
    fclose(file);
    return -1;
}
