#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "item.h"
#include "corner.h"
#include "types.h"

Result addItem(Data *data, Item item);
void fixCorners(Data *data);

void printActiveCornersList(CornersList *cornersList);
void printRoll(Data *data);
float getSuccessRate(Data *data);

int main(int argc, char *argv[])
{
  Data *data;

  if (argc == 2)
  {
    data = initData(argv[1]);
  }
  else
  {
    data = initData("/home/anilkarasah/cdtp/dataset/C1_1");
  }

  for (int i = 0; i < data->numItems; i++)
  {
    addItem(data, data->items[i]);
    fixCorners(data);
    printActiveCornersList(data->cornersList);
  }

  printRoll(data);
  float successRate = getSuccessRate(data);
  printf("Success rate is %.2f%%\n", successRate * 100);

  freeData(data);

  return 0;
}

Result addItem(Data *data, Item item)
{
  // find available corner
  uint8_t cornerIndex = findAvailableCorner(data->cornersList);
  if (cornerIndex >= data->cornersList->numCorners)
  {
    fprintf(stderr, "No available corner found\n");
    return FAILURE;
  }

  // get the corner
  Corner *corner = data->cornersList->corners[cornerIndex];

  // check if the item can be placed at the corner
  if (corner->x + item.width > data->rollWidth || corner->y + item.height > data->rollHeight)
  {
    fprintf(stderr, "Item %d cannot be placed at (%d, %d) 1\n", item.id, corner->x, corner->y);
    return FAILURE;
  }

  Corner fromCorner, toCorner;

  fromCorner = setCornerValues(fromCorner, corner->x, corner->y, false);
  toCorner = setCornerValues(toCorner, corner->x + item.width, corner->y + item.height, false);

  // check if there is any item in between the corner and the item
  Result checkForCrashingItemInBetweenResult = checkForCrashingItemInBetween(data, fromCorner, toCorner);
  if (checkForCrashingItemInBetweenResult == FAILURE)
  {
    fprintf(stderr, "Item %d cannot be placed at (%d, %d) 2\n", item.id, corner->x, corner->y);
    return FAILURE;
  }

  // place the item
  placeItemToTheCorner(data, item, *corner);

  // mark the corner as used
  corner->isUsed = true;

  // append new corners to the list
  Corner *newCorner1 = initCorner(corner->x + item.width, corner->y, false);
  Result checkCorner1PositionAvailableResult = checkCornerPositionAvailable(data, *newCorner1);
  Corner *newCorner2 = initCorner(corner->x, corner->y + item.height, false);
  Result checkCorner2PositionAvailableResult = checkCornerPositionAvailable(data, *newCorner2);

  if (checkCorner1PositionAvailableResult == SUCCESS)
  {
    appendCornerToList(data->cornersList, newCorner1);
  }

  if (checkCorner2PositionAvailableResult == SUCCESS)
  {
    appendCornerToList(data->cornersList, newCorner2);
  }

  printf("Item %d is placed at (%d, %d)\n", item.id, corner->x, corner->y);

  return SUCCESS;
}

void fixCorners(Data *data)
{
  for (int i = 0; i < data->cornersList->numCorners; i++)
  {
    if (data->cornersList->corners[i]->isUsed)
      continue;

    Corner *corner = data->cornersList->corners[i];

    if (data->roll[corner->y][corner->x] != 0)
    {
      corner->isUsed = true;
      continue;
    }
  }
}

void printActiveCornersList(CornersList *cornersList)
{
  printf("\tCorner list:\n\t");
  for (int j = 0; j < cornersList->numCorners; j++)
  {
    if (!cornersList->corners[j]->isUsed)
    {
      printf("(%d, %d) ", cornersList->corners[j]->x, cornersList->corners[j]->y);
    }
  }
  printf("\n");
}

void printRoll(Data *data)
{
  for (int i = 0; i < data->rollHeight; i++)
  {
    for (int j = 0; j < data->rollWidth; j++)
    {
      printf("%2d ", data->roll[i][j]);
    }
    printf("\n");
  }
}

float getSuccessRate(Data *data)
{
  int filledCellCount = 0;

  for (int i = 0; i < data->rollHeight; i++)
  {
    for (int j = 0; j < data->rollWidth; j++)
    {
      if (data->roll[i][j] != 0)
      {
        filledCellCount++;
      }
    }
  }

  float successRate = (float)filledCellCount / (data->rollWidth * data->rollHeight);

  return successRate;
}
