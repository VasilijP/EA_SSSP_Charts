#pragma once

typedef struct {
	int x,y,z;//pozicia vrchola v priestore
	unsigned int deg;//stupen vrcholu (sluzi pre lahsiu manipulaciu)
} VERT;

typedef struct {
	unsigned int v1;
	unsigned int v2;
	unsigned int l;
} EDGE;

typedef struct {
	unsigned int taskId;//id generovane z casu
	unsigned int vertNo;//pocet vrcholov
	unsigned int edgeNo;//pocet hran
	unsigned int src,dst;//zdroj, ciel
	EDGE* E;//hrany, indexovane normalne od 0 po edgeNo-1
	VERT* V;//vrcholy, nulty prvok sa nepocita, vrcholy sa indexuju od 1 po vertNo
} JOB;