//
// Created by Илья Дзензелюк on 03.11.2022.
//

#include "../include/file.h"

bool open_file(char* filename, FILE** file, bool is_writable)
{
  return ((*file = fopen(filename, (is_writable ? "wb" : "rb"))) != NULL);
}

bool close_file(FILE* file)
{
  return (fclose(file) == 0);
}
