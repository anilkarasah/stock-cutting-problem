#include "item.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Data *initData(const char *filename)
{
  Data *data = (Data *)malloc(sizeof(Data));

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
  for (int i = 0; i < data->rollHeight; i++)
  {
    data->roll[i] = (uint8_t *)calloc(data->rollWidth, sizeof(uint8_t));
  }

  return data;
}

void freeData(Data *data)
{
  free(data->items);
  free(data);
}

Result addCorner(Data *data, Corner *corner)
{
  Result cornerPositionAvailableResult = checkCornerPositionAvailable(data, corner);

  if (cornerPositionAvailableResult == FAILURE)
  {
    return FAILURE;
  }

  Result appendCornerToListResult = appendCornerToList(data->cornersList, corner);

  return appendCornerToListResult;
}

Result checkCornerPositionAvailable(Data *data, Corner *corner)
{
  int i = 0;

  if (corner->x >= data->rollWidth || corner->y >= data->rollHeight)
  {
    // out of bounds
    return FAILURE;
  }

  if (data->roll[corner->x][corner->y] != 0)
  {
    // item exists in the position
    return FAILURE;
  }

  while (i < data->cornersList->size && data->cornersList->corners[i] != NULL)
  {
    // traverse through the hole corners list
    // check if corner already exists or, if corner is in the same x-axis or y-axis with any other corner
    // if they are in the same x-axis or y-axis, checks if they contain any item in between

    if (data->cornersList->corners[i]->x == corner->x && data->cornersList->corners[i]->y == corner->y)
    {
      // corner already exists
      return FAILURE;
    }

    if (data->cornersList->corners[i]->x == corner->x)
    {
      // corners are in the same y-axis
      // check if they contain any item in between
      uint8_t fromY;
      uint8_t toY;

      if (data->cornersList->corners[i]->y > corner->y)
      {
        fromY = corner->y;
        toY = data->cornersList->corners[i]->y;
      }
      else
      {
        fromY = data->cornersList->corners[i]->y;
        toY = corner->y;
      }

      Result yAxisAvailableResult = checkForCrashingItemInBetween(data, corner->x, fromY, corner->x, toY);

      if (yAxisAvailableResult == FAILURE)
        return FAILURE;
    }
    else if (data->cornersList->corners[i]->y == corner->y)
    {
      // corners are in the same x-axis
      // check if they contain any item in between

      uint8_t fromX;
      uint8_t toX;

      if (data->cornersList->corners[i]->x > corner->x)
      {
        fromX = corner->x;
        toX = data->cornersList->corners[i]->x;
      }
      else
      {
        fromX = data->cornersList->corners[i]->x;
        toX = corner->x;
      }

      Result xAxisAvailableResult = checkForCrashingItemInBetween(data, fromX, corner->y, toX, corner->y);

      if (xAxisAvailableResult == FAILURE)
        return FAILURE;
    }

    i++;
  }

  return SUCCESS;
}

Result checkForCrashingItemInBetween(Data *data, uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY)
{
  if (fromX >= data->rollWidth || fromY >= data->rollHeight || toX >= data->rollWidth || toY >= data->rollHeight)
  {
    // out of bounds
    return FAILURE;
  }

  for (int i = fromX; i <= toX; i++)
  {
    int j = fromY;
    while (j <= toY && data->roll[i][j] == 0)
      j++;

    if (j <= toY)
    {
      // there is an item in between
      return FAILURE;
    }
  }

  return SUCCESS;
}

void placeItemToTheCorner(Data *data, Item item, Corner *corner)
{
  for (int i = corner->x; i < corner->x + item.width; i++)
    for (int j = corner->y; j < corner->y + item.height; j++)
      data->roll[i][j] = item.id;
}
