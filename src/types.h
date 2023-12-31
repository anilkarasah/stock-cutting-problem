#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  SUCCESS = 0,
  FAILURE = 1,
} Result;

typedef struct
{
  uint8_t x;
  uint8_t y;
  bool isUsed;
} Corner;

typedef struct
{
  Corner **corners;
  uint16_t size;
  uint16_t numCorners;
} CornersList;

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

#endif
