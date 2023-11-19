#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "item.h"
#include "corner.h"

#define FILENAME_BUFFER_SIZE 64

typedef enum
{
  SUCCESS = 0,
  FAILURE = 1,
} Result;

// corner related helper functions
Result addCorner(Data *data, Corner corner);
Result checkCornerPositionAvailable(Data *data, Corner corner);

// roll related helper functions
Result checkForCrashingItemInBetween(Data *data, uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY);

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

  Result cornerPositionAvailableResult = checkCornerPositionAvailable(data, corner);

  if (cornerPositionAvailableResult == FAILURE)
  {
    return FAILURE;
  }

  data->cornersList->corners[data->cornersList->numberOfCorners] = corner;
  data->cornersList->numberOfCorners++;

  return SUCCESS;
}

Result checkCornerPositionAvailable(Data *data, Corner corner)
{
  int i = 0;

  if (corner.x >= data->rollWidth || corner.y >= data->rollHeight)
  {
    // out of bounds
    return FAILURE;
  }

  if (data->roll[corner.x][corner.y] != 0)
  {
    // item exists in the position
    return FAILURE;
  }

  while (i < data->cornersList->numberOfCorners)
  {
    // traverse through the hole corners list
    // check if corner already exists or, if corner is in the same x-axis or y-axis with any other corner
    // if they are in the same x-axis or y-axis, checks if they contain any item in between

    if (data->cornersList->corners[i].x == corner.x && data->cornersList->corners[i].y == corner.y)
    {
      // corner already exists
      return FAILURE;
    }

    if (data->cornersList->corners[i].x == corner.x)
    {
      // corners are in the same y-axis
      // check if they contain any item in between
      uint8_t fromY;
      uint8_t toY;

      if (data->cornersList->corners[i].y > corner.y)
      {
        fromY = corner.y;
        toY = data->cornersList->corners[i].y;
      }
      else
      {
        fromY = data->cornersList->corners[i].y;
        toY = corner.y;
      }

      Result yAxisAvailableResult = checkForCrashingItemInBetween(data, corner.x, fromY, corner.x, toY);

      if (yAxisAvailableResult == FAILURE)
        return FAILURE;
    }
    else if (data->cornersList->corners[i].y == corner.y)
    {
      // corners are in the same x-axis
      // check if they contain any item in between

      uint8_t fromX;
      uint8_t toX;

      if (data->cornersList->corners[i].x > corner.x)
      {
        fromX = corner.x;
        toX = data->cornersList->corners[i].x;
      }
      else
      {
        fromX = data->cornersList->corners[i].x;
        toX = corner.x;
      }

      Result xAxisAvailableResult = checkForCrashingItemInBetween(data, fromX, corner.y, toX, corner.y);

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
