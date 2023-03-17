#include "algebra.h"
#include "mesh.h"
#include "triangle.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int process_vertex(const char* line_buf, Vector3* v)
{
    const char* delim = " ";
    char line[256];
    strcpy(line, line_buf);
    char* token = strtok(line, delim);
    //skip 'v'
    token = strtok(NULL, delim);

    float coords[3];
    int i = 0;
    while (token != NULL && i < 3) {
        coords[i] = atof(token);
        token = strtok(NULL, delim);
        i++;
    }

    if (i == 3 && token == NULL) {
        v->x = coords[0];
        v->y = coords[1];
        v->z = coords[2];
        return 0;
    }

    puts("Parsing error: could not parse vertex");
    return -1;
}

int process_face(const char* line_buf, uint32_t* vert_ids)
{
    const char* delim = " ";
    char line[256];
    strcpy(line, line_buf);
    char* token = strtok(line, delim);
    //skip 'f'
    token = strtok(NULL, delim);

    char* vert_defs[3] = { 0, 0, 0 };

    int i = 0;
    while (token != NULL && i < 3) {
        vert_defs[i] = malloc(16);
        strcpy(vert_defs[i], token);
        token = strtok(NULL, delim);
        i++;
    }

    for (int i = 0; i < 3; i++) {
        if (vert_defs[i] == NULL) {
            return -1;
        }
        char* token = strtok(vert_defs[i], "/");
        int id = atoi(token);
        vert_ids[i] = id;
        free(vert_defs[i]);
    }

    return 0;
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
            if (result != 0) {
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
            if (result == 0) {
                Triangle t = { verts[vert_ids[0] - 1], verts[vert_ids[1] - 1], verts[vert_ids[2] - 1], { ColorGreen, ColorRed, ColorBlue } };
                triangles[t_i] = t;
                t_i++;
            }
        }
    }

    mesh->triangle_count = t_i;
    mesh->triangles = triangles;

    return 0;

error:
    fclose(file);
    return -1;
}
