#include "item.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Data *initData(const char *filename)
{
  int i;
  Data *data = (Data *)malloc(sizeof(Data));

  // open file read stream
  FILE *fp;
  if ((fp = fopen(filename, "r")) == NULL)
  {
    fprintf(stderr, "Error opening file %s\n", filename);
    exit(-1);
  }

  // read number of items, roll width, roll height data from header
  fscanf(fp, "%d", &data->numItems);
  fscanf(fp, "%d %d", &data->rollWidth, &data->rollHeight);

  // allocate memory for items
  data->items = (Item *)malloc(sizeof(Item) * data->numItems);
  
  data->cornersList = createCornersList(data->numItems * 2);

  // read item data
  for (i = 0; i < data->numItems; i++)
  {
    fscanf(fp, "%d %d", &data->items[i].width, &data->items[i].height);
    data->items[i].index = i + 1;
  }

  // close file read stream
  fclose(fp);

  return data;
}

void freeData(Data *data)
{
  free(data->items);
  free(data);
}
