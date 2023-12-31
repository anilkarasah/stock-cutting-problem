#ifndef EDGE_H
#define EDGE_H

#include <stdint.h>
#include "types.h"

#define EDGE_CELL(cell) ((cell) * 2 + 1)
#define EDGE_VALUE UINT8_MAX

uint8_t **prepareEdgeMatrix(Data *data);
void calculateEdges(Data *data, uint8_t **edgeMatrix);
void printEdges(uint8_t **edgeMatrix, int width, int height);

#endif
