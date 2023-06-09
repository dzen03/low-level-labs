//
// Created by Илья Дзензелюк on 22.10.2022.
//

#ifndef ASSIGNMENT_3_IMAGE_ROTATION_SOLUTION_INCLUDE_IMAGE_H_
#define ASSIGNMENT_3_IMAGE_ROTATION_SOLUTION_INCLUDE_IMAGE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct __attribute__((packed)) pixel
{
  uint8_t b, g, r;
};

struct image
{
  uint64_t width, height;
  struct pixel* data;
};

bool rotate_image(struct image* in);

#endif //ASSIGNMENT_3_IMAGE_ROTATION_SOLUTION_INCLUDE_IMAGE_H_
