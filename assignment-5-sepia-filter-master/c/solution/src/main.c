#include "bmp.h"
#include "file.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char** argv ) {
  if (argc != 3)
  {
    printf("You need 2 args to execute!\n");
    return 0;
  }
  printf("from: %s\nto: %s\n", argv[1] , argv[2]);

  FILE *fin = NULL;
  if (!open_file(argv[1], &fin, false))
  {
    fprintf(stderr, "Can't open %s", argv[1]);
    return 0;
  }

  struct image img = {0, 0, NULL};

  int status;
  if ((status = from_bmp(fin, &img)) != READ_OK)
  {
    fprintf(stderr, "Error (%d) during reading", status);
    return 0;
  }

  FILE *fout = NULL;
  if (!open_file(argv[2], &fout, true))
  {
    fprintf(stderr, "Can't open %s", argv[2]);
    return 0;
  }
  if (!apply_sepia(&img))
  {
    fprintf(stderr, "Error during sepia");
    return 0;
  }

  if ((status = to_bmp(fout, &img)) != READ_OK)
  {
    fprintf(stderr, "Error (%d) during writing", status);
    return 0;
  }

  free(img.data);

  close_file(fin);
  close_file(fout);
  return 0;
}
