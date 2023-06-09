//
// Created by Илья Дзензелюк on 22.10.2022.
//

#ifndef ASSIGNMENT_3_IMAGE_ROTATION_SOLUTION_INCLUDE_BMP_H_
#define ASSIGNMENT_3_IMAGE_ROTATION_SOLUTION_INCLUDE_BMP_H_

#include "image.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct __attribute__((packed)) bmp_header
{
  uint16_t bfType;
  uint32_t bfileSize;
  uint32_t bfReserved;
  uint32_t bOffBits;
  uint32_t biSize;
  uint32_t biWidth;
  uint32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  uint32_t biXPelsPerMeter;
  uint32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
};


/*  deserializer   */
enum read_status  {
  READ_OK = 0,
  READ_INVALID_SIGNATURE,
  READ_INVALID_BITS,
  READ_INVALID_HEADER,
  MEMORY_ERROR
};

enum read_status from_bmp( FILE* in, struct image* img );

/*  serializer   */
enum  write_status  {
  WRITE_OK = 0,
  WRITE_ERROR
  /* коды других ошибок  */
};

enum write_status to_bmp( FILE* out, const struct image* img );

struct bmp_header create_header(uint32_t width, uint32_t height);


#endif //ASSIGNMENT_3_IMAGE_ROTATION_SOLUTION_INCLUDE_BMP_H_

