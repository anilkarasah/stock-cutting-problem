#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "item.h"
#include "corner.h"

#define FILENAME_BUFFER_SIZE 64

typedef enum {
  SUCCESS = 0,
  FAILURE = 1,
} Result;

Result addCorner(Data *data, Corner corner);

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

  freeData(data);

  return 0;
}

Result addCorner(Data *data, Corner corner)
{
  if (data->cornersList->numberOfCorners == data->cornersList->size)
  {
    return FAILURE;
  }

  data->cornersList->corners[data->cornersList->numberOfCorners] = corner;
  data->cornersList->numberOfCorners++;
  
  return SUCCESS;
}
