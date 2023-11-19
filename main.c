#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "item.h"

#define FILENAME_BUFFER_SIZE 64

int main(int argc, char *argv[])
{
  Data *data;
  char filename[FILENAME_BUFFER_SIZE] = "dataset/";

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(-1);
  }

  strcat(filename, argv[1]);

  data = initData(filename);

  binPackingBFD(data);
  
  cuttingStockBL(data);

  freeData(data);

  return 0;
}
