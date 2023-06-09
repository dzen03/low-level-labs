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

bool apply_sepia(struct image* img)
{
  for (int i = 0; i < img->height; ++i)
    for (int j = 0; j < img->width; ++j)
    {
      uint8_t r = img->data[i * img->width + j].r;
      uint8_t g = img->data[i * img->width + j].g;
      uint8_t b = img->data[i * img->width + j].b;


      img->data[i * img->width + j].r = (int)(min(r * 0.393 + g * 0.769
        + b * 0.189, 255));
      img->data[i * img->width + j].g = (int)(min(r * 0.349 + g * 0.686
        + img->data[i * img->width + j].b * 0.168, 255));
      img->data[i * img->width + j].b = (int)(min(r * 0.272 + g * 0.534
        + b * 0.131, 255));
    }

  return true;
}