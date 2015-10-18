#pragma once
//procedurky pouzivane v roznych castiach programu

#include <math.h>
#include "types.h"

//nerekurzivna verzia quicksortu P su prvky, s,e je rozsah, l je limit, pokial staci zotriedit
void qsortNR(unsigned int * P, int s, int e, int l);

//pocita vzdialenost vrcholov
double distance(VERT a, VERT b);

//pocita vzdialenost vrcholov ako unsigned int
unsigned int distance_ui(VERT a, VERT b);