#pragma once
//procedurky pouzivane v roznych castiach programu

#include <math.h>
#include "types.h"

//porovnava dva unsigned inty, hodi sa pre qsort
int compare(const void * a,const void * b);

//pocita vzdialenost vrcholov
double distance(VERT a, VERT b);

//pocita vzdialenost vrcholov ako unsigned int
unsigned int distance_ui(VERT a, VERT b);