//
// Created by Илья Дзензелюк on 22.10.2022.
//

#include "../include/bmp.h"


enum write_status to_bmp(FILE *out, const struct image *img) {
  if (out == NULL)
    return WRITE_FILE_ERROR;
  if (img == NULL)
    return WRITE_IMAGE_ERROR;

  struct bmp_header header = create_header(img->width, img->height);

  fwrite(&header, sizeof(struct bmp_header), 1, out);

  uint32_t row_size = (uint32_t) ceil((double) (header.biBitCount * header.biWidth) / 32.) * 4;

  uint32_t padding_size = row_size - header.biWidth * sizeof(struct pixel);

  int8_t* padding_data = malloc(padding_size);
  for (size_t i = 0; i < padding_size; ++i)
  {
    padding_data[i] = 0;
  }


  for (int i = 0; i < header.biHeight; ++i)
  {
    size_t tmp = fwrite(img->data + i * header.biWidth, sizeof(struct pixel),  header.biWidth, out);
    if (tmp == 0)
    {
      free(padding_data);
      return WRITE_ERROR;
    }

    tmp = fwrite(padding_data, padding_size, 1, out);
    if (tmp == 0)
    {
      free(padding_data);
      return WRITE_ERROR;
    }
  }

  free(padding_data);
  return WRITE_OK;
}

enum read_status from_bmp(FILE *in, struct image *img) {
  struct bmp_header *header = malloc(sizeof(struct bmp_header));

  if (header == NULL)
  {
    return MEMORY_ERROR;
  }

  size_t tmp = fread(header, sizeof(struct bmp_header), 1, in);
  if (!tmp || header->biBitCount != sizeof(struct pixel) * 8)
  {
    free(header);
    return READ_INVALID_HEADER;
  }

  uint32_t row_size = (uint32_t) ceil(header->biBitCount * header->biWidth / 32.) * 4;

  struct pixel* data = malloc(sizeof(struct pixel) * header->biHeight * header->biWidth);
  if (data == NULL)
  {
    free(header);
    return MEMORY_ERROR;
  }

  uint32_t padding_size = row_size - header->biWidth * sizeof(struct pixel);

  struct pixel* padding_data = malloc(padding_size);

  if (padding_data == NULL)
  {
    free(data);
    free(header);
    return MEMORY_ERROR;
  }


  for (int i = 0; i < header->biHeight; ++i)
  {
    if (fread(data + i * header->biWidth, sizeof(struct pixel),  header->biWidth, in) == 0)
    {
      free(padding_data);
      free(header);
      free(data);
      return READ_INVALID_BITS;
    }

    if (fread(padding_data, padding_size, 1, in) == 0)
    {
      free(padding_data);
      free(header);
      free(data);
      return READ_INVALID_BITS;
    }
  }


  img->data = data;
  img->height = header->biHeight;
  img->width = header->biWidth;

  free(padding_data);
  free(header);

  return READ_OK;
}

#define bfType 19778
#define bfReserved 0
#define biSize 40
#define biPlanes 1
#define biCompression 0


struct bmp_header create_header(uint32_t width, uint32_t height) {
  uint32_t row_size_byte = (uint32_t) ceil((double) (sizeof(struct pixel) * 8 * width) / 32.) * 4;
  return (struct bmp_header) {bfType, (height * row_size_byte) + sizeof(struct bmp_header), bfReserved,
                              sizeof(struct bmp_header), biSize,
                              width, height, biPlanes, sizeof(struct pixel) * 8, biCompression, height * row_size_byte};
}
