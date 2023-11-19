#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "item.h"
#include "corner.h"
#include "types.h"

Result addItem(Data *data, Item item);

int main(int argc, char *argv[])
{
  Data *data;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(-1);
  }

  data = initData(argv[1]);

  for (int i = 0; i < data->numItems; i++)
  {
    addItem(data, data->items[i]);
  }

  freeData(data);

  return 0;
}

Result addItem(Data *data, Item item)
{
  // find available corner
  uint8_t cornerIndex = findAvailableCorner(data->cornersList);
  if (cornerIndex == UINT8_MAX)
    return FAILURE;

  // get the corner
  Corner *corner = data->cornersList->corners[cornerIndex];

  // check if the item can be placed at the corner
  if (corner->x + item.width > data->rollWidth || corner->y + item.height > data->rollHeight)
    return FAILURE;

  // check if there is any item in between the corner and the item
  Result checkForCrashingItemInBetweenResult = checkForCrashingItemInBetween(data, corner->x, corner->y, corner->x + item.width, corner->y + item.height);
  if (checkForCrashingItemInBetweenResult == FAILURE)
    return FAILURE;

  // place the item
  placeItemToTheCorner(data, item, corner);

  // mark the corner as used
  corner->isUsed = true;

  printf("Item %d is placed at (%d, %d)\n", item.id, corner->x, corner->y);

  return SUCCESS;
}
