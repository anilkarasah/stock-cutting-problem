#include "item.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// read data from file and initialize data structure
Data *initData(const char *filename)
{
  Data *data = (Data *)malloc(sizeof(Data));
  if (data == NULL)
  {
    fprintf(stderr, "Error allocating memory for data\n");
    exit(-1);
  }

  // open file read stream
  FILE *fp;
  if ((fp = fopen(filename, "r")) == NULL)
  {
    fprintf(stderr, "Error opening file %s\n", filename);
    exit(-1);
  }

  // read number of items, roll width, roll height data from header
  int numItems, rollWidth, rollHeight;
  fscanf(fp, "%d", &numItems);
  fscanf(fp, "%d %d", &rollWidth, &rollHeight);
  data->numItems = numItems;
  data->rollWidth = rollWidth;
  data->rollHeight = rollHeight;

  // allocate memory for items
  data->items = (Item *)malloc(sizeof(Item) * data->numItems);
  if (data->items == NULL)
  {
    fprintf(stderr, "Error allocating memory for data->items\n");
    exit(-1);
  }

  // read item data
  for (int i = 0; i < data->numItems; i++)
  {
    int width, height;
    fscanf(fp, "%d %d", &width, &height);
    data->items[i].width = width;
    data->items[i].height = height;
    data->items[i].id = i + 1;
  }

  // close file read stream
  fclose(fp);

  // initialize corners list
  data->cornersList = createCornersList(data->numItems * 2);

  // initialize roll matrix
  data->roll = (uint8_t **)malloc(sizeof(uint8_t *) * data->rollHeight);
  if (data->roll == NULL)
  {
    fprintf(stderr, "Error allocating memory for data->roll\n");
    exit(-1);
  }

  for (int i = 0; i < data->rollHeight; i++)
  {
    data->roll[i] = (uint8_t *)calloc(data->rollWidth, sizeof(uint8_t));
    if (data->roll[i] == NULL)
    {
      fprintf(stderr, "Error allocating memory for data->roll[%d]\n", i);
      exit(-1);
    }
  }

  return data;
}

// free all allocated memory
void freeData(Data *data)
{
  freeCornersList(data->cornersList);

  for (int i = 0; i < data->rollHeight; i++)
  {
    free(data->roll[i]);
  }

  free(data->items);
  free(data);
}

// check if the corner can be placed, if so, append it to the list
Result checkAvailableThenAppendCorner(Data *data, Corner *corner)
{
  Result cornerPositionAvailableResult = checkCornerPositionAvailable(data, *corner);

  if (cornerPositionAvailableResult == FAILURE)
  {
    // corner cannot be placed
    return FAILURE;
  }

  Result appendCornerToListResult = appendCornerToList(data->cornersList, corner);
  return appendCornerToListResult;
}

// check if the corner can be placed to the roll matrix in the given position
Result checkCornerPositionAvailable(Data *data, Corner appendingCorner)
{
  if (appendingCorner.x >= data->rollWidth || appendingCorner.y >= data->rollHeight)
  {
    // out of bounds
    fprintf(stderr, "Corner (%d, %d) is out of bounds\n", appendingCorner.x, appendingCorner.y);
    return FAILURE;
  }

  if (data->roll[appendingCorner.y][appendingCorner.x] != 0)
  {
    // item exists in the position
    fprintf(stderr, "Item exists in the position (%d, %d)\n", appendingCorner.x, appendingCorner.y);
    return FAILURE;
  }

  for (int i = 0; i < data->cornersList->numCorners; i++)
  {
    Corner *currentCorner = data->cornersList->corners[i];

    if (currentCorner->isUsed)
      continue;

    // traverse through the hole corners list
    // check if corner already exists or, if corner is in the same x-axis or y-axis with any other corner
    // if they are in the same x-axis or y-axis, checks if they contain any item in between

    if (currentCorner->x == appendingCorner.x && currentCorner->y == appendingCorner.y)
    {
      // corner already exists
      fprintf(stderr, "Corner (%d, %d) already exists\n", appendingCorner.x, appendingCorner.y);
      return FAILURE;
    }
  }

  // this corner can be placed
  return SUCCESS;
}

// check if there is any item in between the two corners
Result checkForCrashingItemInBetween(Data *data, Corner fromCorner, Corner toCorner)
{
  if (fromCorner.x > data->rollWidth || fromCorner.y > data->rollHeight || toCorner.x > data->rollWidth || toCorner.y > data->rollHeight)
  {
    // out of bounds, cannot check
    return FAILURE;
  }

  if (fromCorner.x == toCorner.x)
  {
    // same x-axis, only check if there is any item in between in the y-axis
    int i = fromCorner.y;
    while (i < toCorner.y && data->roll[i][fromCorner.x] == 0)
      i++;

    if (i < toCorner.y)
    {
      // there is an item in between
      return FAILURE;
    }
  }
  else if (fromCorner.y == toCorner.y)
  {
    // same y-axis, only check if there is any item in between in the x-axis
    int i = fromCorner.x;
    while (i < toCorner.x && data->roll[fromCorner.y][i] == 0)
      i++;

    if (i < toCorner.x)
    {
      // there is an item in between
      return FAILURE;
    }
  }
  else
  {
    // different x-axis and y-axis, check if there is any item in between the two corners
    for (int i = fromCorner.y; i < toCorner.y; i++)
    {
      int j = fromCorner.x;
      while (j < toCorner.x && data->roll[i][j] == 0)
        j++;

      if (j < toCorner.x)
      {
        // there is an item in between
        return FAILURE;
      }
    }
  }

  return SUCCESS;
}

// fills roll matrix with item id
void placeItemToTheCorner(Data *data, Item item, Corner corner)
{
  for (int i = corner.y; i < corner.y + item.height; i++)
    for (int j = corner.x; j < corner.x + item.width; j++)
      data->roll[i][j] = item.id;
}
