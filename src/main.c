#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "item.h"
#include "corner.h"
#include "types.h"
#include "edge.h"
#include "graph.h"

#define DISTANCE_UNIT 5

Result processItem(Data *data, Item item);
void fixCorners(Data *data);

void printActiveCornersList(CornersList *cornersList);
void printRoll(Data *data);
float calculateSuccessRate(Data *data);

void generateGcode(EdgeList *edgeList);

int main(int argc, char *argv[])
{
  Data *data;

  if (argc == 2)
  {
    // filename is provided from terminal
    data = initData(argv[1]);
  }
  else
  {
    // use default file
    data = initData("/home/anilkarasah/cdtp/dataset/C1_1");
  }

  // process each item
  for (int i = 0; i < data->numItems; i++)
  {
    processItem(data, data->items[i]);

    // remove unappropriate corners
    fixCorners(data);

    // (optional) print active corners list (corners that are not used yet)
    // printActiveCornersList(data->cornersList);
  }

  // print the final roll
  // printRoll(data);

  // print success rate
  // float successRate = calculateSuccessRate(data);
  // printf("Success rate is %.2f%%\n", successRate * 100);

  // calculate edge matrix
  uint8_t **edgeMatrix = prepareEdgeMatrix(data);
  processEdgeMatrix(data, edgeMatrix);
  printEdgeMatrix2(edgeMatrix, EDGE_CELL(data->rollWidth), EDGE_CELL(data->rollHeight));

  EdgeList *cornerAndEdgeList = (EdgeList *)malloc(sizeof(EdgeList));
  cornerAndEdgeList->numEdges = 0;
  cornerAndEdgeList->edgeList = (Edge *)malloc(data->numItems * 4 * sizeof(Edge));
  Corner fromCorner = {0, 0, false, 0};
  Corner currentCorner = {0, 0, false, 0};
  calculateVertexAndEdgeList(data, cornerAndEdgeList, edgeMatrix, fromCorner, currentCorner);
  CornersList *vertexList = calculateVertexList(data, cornerAndEdgeList);

  generateGcode(cornerAndEdgeList);

  for (int i = 0; i < vertexList->numCorners; i++)
  {
    printf("Vertex %2d: (%2d,%2d)\n",
           i + 1,
           vertexList->corners[i]->x,
           vertexList->corners[i]->y);
  }

  // calculate optimal graph traverse for Gcode
  uint8_t **adjacencyMatrix = generateAdjacencyMatrix(vertexList, cornerAndEdgeList);

  printf("%3s", "");
  for (int i = 0; i < vertexList->numCorners; i++)
  {
    printf("%3d", i + 1);
  }

  printf("\n");

  for (int i = 0; i < vertexList->numCorners; i++)
  {
    printf("%3d", i + 1);
    for (int j = 0; j < vertexList->numCorners; j++)
    {
      printf("%3s", adjacencyMatrix[i][j] ? "#" : "");
    }
    printf("\n");
  }

  bool *visited = (bool *)calloc(vertexList->numCorners, sizeof(bool));
  dfsTraverse(adjacencyMatrix, vertexList, data->rollWidth, 0, 0, visited);

  return 0;
}

Result processItem(Data *data, Item item)
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
  setCornerValues(&fromCorner, corner->x, corner->y, false, data->rollWidth);
  setCornerValues(&toCorner, corner->x + item.width, corner->y + item.height, false, data->rollWidth);

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

  // create new corners
  Corner *topRightCorner = initCorner(corner->x + item.width, corner->y, false, data->rollWidth);
  Corner *bottomLeftCorner = initCorner(corner->x, corner->y + item.height, false, data->rollWidth);

  // append new corners to the list
  Result checkTopRightCornerPositionAvailableResult = checkAvailableThenAppendCorner(data, topRightCorner);
  Result checkBottomLeftCornerPositionAvailableResult = checkAvailableThenAppendCorner(data, bottomLeftCorner);

  if (checkTopRightCornerPositionAvailableResult == FAILURE)
    free(topRightCorner);

  if (checkBottomLeftCornerPositionAvailableResult == FAILURE)
    free(bottomLeftCorner);

  printf("Item %d (%dx%d) is placed at (%d, %d)\n", item.id, item.width, item.height, corner->x, corner->y);

  return SUCCESS;
}

void fixCorners(Data *data)
{
  // remove corners that are not used yet
  // but, have an item in its position
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

float calculateSuccessRate(Data *data)
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

void generateGcode(EdgeList *edgeList)
{
  FILE *fp;

  if ((fp = fopen("out.gcode", "w")) == NULL)
  {
    printf("Error! opening file");
    exit(1);
  }

  fprintf(fp, "G90\n");
  fprintf(fp, "G21\n");
  fprintf(fp, "G1 F1000\n");

  for (int i = 0; i < edgeList->numEdges; i++)
  {
    Edge current = edgeList->edgeList[i];

    if (i > 0 && (current.fromCorner.x != edgeList->edgeList[i - 1].toCorner.x || current.fromCorner.y != edgeList->edgeList[i - 1].toCorner.y))
    {
      // start writing mode by decreasing z (aka. pen down)
      fprintf(fp, "G1 Z0\n");
    }

    fprintf(fp, "G1 X%d Y%d\n", current.fromCorner.x * DISTANCE_UNIT, current.fromCorner.y * DISTANCE_UNIT);
    fprintf(fp, "G1 X%d Y%d\n", current.toCorner.x * DISTANCE_UNIT, current.toCorner.y * DISTANCE_UNIT);

    if (i < edgeList->numEdges - 1 && (current.toCorner.x != edgeList->edgeList[i + 1].fromCorner.x || current.toCorner.y != edgeList->edgeList[i + 1].fromCorner.y))
    {
      // end writing mode by increasing z (aka. pen up)
      fprintf(fp, "G1 Z%d\n", DISTANCE_UNIT);
    }
  }
}
