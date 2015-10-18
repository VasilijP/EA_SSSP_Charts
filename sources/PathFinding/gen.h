#pragma once

#include "types.h"

/*PREPINACE pre generovanie ulohy*/
//rozmiestnenie vrcholov v guli
#define SPACE 1
//rozmiestnenie vrcholov na kruhu
#define FLAT 2
//prepajanie len s najblizsimi susedmi
#define NEAREST 4
//percentualne definovanie prepojenosti
#define PEREDGECONN 8
//definovanie na zaklade rozsahu
#define MINEDGECONN 16

void genJob(unsigned int flags,unsigned int vertNo, unsigned int eLenAdd, unsigned int conn);

void genJob(unsigned int flags,unsigned int vertNo, unsigned int eLenAdd, unsigned int conn, unsigned int taskId);

JOB * getJob();