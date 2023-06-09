//
// Created by Илья Дзензелюк on 03.11.2022.
//

#ifndef ASSIGNMENT_3_IMAGE_ROTATION_SOLUTION_SRC_FILE_H_
#define ASSIGNMENT_3_IMAGE_ROTATION_SOLUTION_SRC_FILE_H_

#include <stdbool.h>
#include <stdio.h>
#include <strings.h>

bool open_file(char* filename, FILE** file, bool is_writable);

bool close_file(FILE* file);

#endif //ASSIGNMENT_3_IMAGE_ROTATION_SOLUTION_SRC_FILE_H_
