//
// Created by Илья Дзензелюк on 18.11.2022.
//
#include "../include/image.h"

bool rotate_image(struct image* img)
{
  struct image tmp = {img->height, img->width, malloc(sizeof(struct pixel) * img->height * img->width)};

  if (tmp.data == NULL)
    return false;

  for (int i = 0; i < img->height; ++i)
    for (int j = 0; j < img->width; ++j)
    {
      tmp.data[j * img->height + (img->height - i - 1)] = img->data[i * img->width + j];
    }

  free(img->data);
  *img = tmp;
  return true;
}
