#ifndef EDGE_H
#define EDGE_H

#include <stdint.h>
#include "types.h"

#define EDGE_CELL(cell) ((cell) * 2 + 1)
#define EDGE_VALUE UINT8_MAX

// edge matrix related functions
uint8_t **prepareEdgeMatrix(Data *data);
void processEdgeMatrix(Data *data, uint8_t **edgeMatrix);
void printEdgeMatrix(uint8_t **edgeMatrix, int width, int height);
void printEdgeMatrix2(uint8_t **edgeMatrix, int width, int height);

// edge list related functions
EdgeList *calculateEdgeList(Data *data, uint8_t **edgeMatrix);
CornersList *calculateVertexList(Data *data, uint8_t **edgeMatrix, EdgeList *edgeList);

#endif
