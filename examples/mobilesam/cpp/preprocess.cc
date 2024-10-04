// Copyright (c) 2024 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocess.h"

#define MAX_TEXT_LINE_LENGTH 1024

int count_lines(FILE* file)
{
    int count = 0;
    char ch;

    while(!feof(file))
    {
        ch = fgetc(file);
        if(ch == '\n')
        {
            count++;
        }
    }
    count += 1;

    rewind(file);
    return count;
}

float* read_coords_from_file(const char* filename, int* line_count)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return NULL;
    }

    int num_lines = count_lines(file);
    printf("num_lines=%d\n", num_lines);
    float* coords = (float*)malloc(num_lines * 2 * sizeof(float));
    memset(coords, 0, num_lines * 2 * sizeof(float));

    char buffer[MAX_TEXT_LINE_LENGTH];
    int index = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = ' ';  // 移除换行符

        char* coord = strtok(buffer, " ");
        while (coord != NULL) 
        {
            coords[index++] = atof(coord);
            coord = strtok(NULL, " ");
        }
    }

    fclose(file);

    *line_count = num_lines;
    return coords;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int get_preprocess_shape(int ori_heigth, int ori_width, int* new_shape)
{
    float scale = IMG_SIZE * 1.0 / max(ori_heigth, ori_width);

    int new_height = ori_heigth * scale + 0.5;
    int new_width = ori_width * scale + 0.5;

    new_shape[0] = new_height;
    new_shape[1] = new_width;

    return 0;
}

int point_coords_preprocess(float* ori_point_coords, int coords_size, int ori_height, int ori_width, float* cvt_point_coords)
{
    int* new_shape = (int*)malloc(2 * sizeof(int));

    get_preprocess_shape(ori_height, ori_width, new_shape);

    for (int i = 0; i < coords_size; i+=2)
    {
        cvt_point_coords[i] = ori_point_coords[i] * (new_shape[1] * 1.0 / ori_width);
        cvt_point_coords[i + 1] = ori_point_coords[i + 1] * (new_shape[0] * 1.0 / ori_height);
    }

    if (new_shape != NULL)
    {
        free(new_shape);
    }

    return 0;
}