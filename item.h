#ifndef ITEM_H
#define ITEM_H

#include <stdint.h>
#include "corner.h"

// Structure for items
typedef struct
{
  uint8_t width;
  uint8_t height;
  uint8_t id;
} Item;

// Structure for data read from file
typedef struct
{
  uint8_t numItems;
  uint8_t rollWidth;
  uint8_t rollHeight;
  Item *items;
  CornersList *cornersList;
  uint8_t **roll;
} Data;

// dataset related functions
Data *initData(const char *filename);
void freeData(Data *data);

#endif