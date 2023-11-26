#include "item.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void freeData(Data *data)
{
  for (int i = 0; i < data->rollHeight; i++)
  {
    free(data->roll[i]);
  }

  free(data->items);
  free(data);
}

Result addCorner(Data *data, Corner *corner)
{
  Result cornerPositionAvailableResult = checkCornerPositionAvailable(data, *corner);

  if (cornerPositionAvailableResult == FAILURE)
  {
    return FAILURE;
  }

  Result appendCornerToListResult = appendCornerToList(data->cornersList, corner);

  return appendCornerToListResult;
}

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

    uint8_t fromX = appendingCorner.x;
    uint8_t toX = appendingCorner.x;
    uint8_t fromY = appendingCorner.y;
    uint8_t toY = appendingCorner.y;

    if (currentCorner->x == appendingCorner.x)
    {
      // corners are in the same y-axis
      // check if they contain any item in between

      if (currentCorner->y > appendingCorner.y)
      {
        fromY = appendingCorner.y;
        toY = currentCorner->y;
      }
      else
      {
        fromY = currentCorner->y;
        toY = appendingCorner.y;
      }
    }
    else if (currentCorner->y == appendingCorner.y)
    {
      // corners are in the same x-axis
      // check if they contain any item in between

      if (currentCorner->x > appendingCorner.x)
      {
        fromX = appendingCorner.x;
        toX = currentCorner->x;
      }
      else
      {
        fromX = currentCorner->x;
        toX = appendingCorner.x;
      }
    }
    else
    {
      // corners are not in the same axis
      continue;
    }

    Corner fromCorner, toCorner;

    fromCorner = setCornerValues(fromCorner, fromX, fromY, false);
    toCorner = setCornerValues(toCorner, toX, toY, false);

    Result cornerAvailableResult = checkForCrashingItemInBetween(data, fromCorner, toCorner);

    if (cornerAvailableResult == FAILURE)
    {
      // there is an item in between
      fprintf(stderr, "There is an item in between (%d, %d) and (%d, %d)\n", fromX, fromY, toX, toY);
      return FAILURE;
    }

    // they are in the same axis and there is no item in between
    // keep the lower positioned corner, and remove the higher positioned corner
    if (currentCorner->x == appendingCorner.x)
    {
      if (currentCorner->y > appendingCorner.y)
        // they are in the same x-axis and current corner is higher than appending corner
        // this is an acceptable situation, but the other corner should not be used
        currentCorner->isUsed = true;
      else
        // they are in the same x-axis and current corner is lower than appending corner
        return FAILURE;
    }
    else if (currentCorner->y == appendingCorner.y)
    {
      if (currentCorner->x > appendingCorner.x)
        // they are in the same y-axis and current corner is higher than appending corner
        // this is an acceptable situation, but the other corner should not be used
        currentCorner->isUsed = true;
      else
        // they are in the same y-axis and current corner is lower than appending corner
        return FAILURE;
    }
  }

  // this corner can be placed
  return SUCCESS;
}

Result checkForCrashingItemInBetween(Data *data, Corner fromCorner, Corner toCorner)
{
  if (fromCorner.x > data->rollWidth || fromCorner.y > data->rollHeight || toCorner.x > data->rollWidth || toCorner.y > data->rollHeight)
  {
    // out of bounds
    return FAILURE;
  }

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

  return SUCCESS;
}

void placeItemToTheCorner(Data *data, Item item, Corner corner)
{
  for (int i = corner.y; i < corner.y + item.height; i++)
    for (int j = corner.x; j < corner.x + item.width; j++)
      data->roll[i][j] = item.id;
}
