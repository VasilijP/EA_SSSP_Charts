
#include "utils.h"

int compare(const void * a,const void * b){
	if (*(unsigned int*)a < *(unsigned int*)b) return(-1);
	if (*(unsigned int*)a > *(unsigned int*)b) return(1);
	return(0);
	//return ((*(unsigned int*)a)-(*(unsigned int*)b));
}

double distance(VERT a, VERT b){
	double x = a.x-b.x;
	double y = a.y-b.y;
	double z = a.z-b.z;
	return(sqrt(x*x+y*y+z*z));
}

unsigned int distance_ui(VERT a, VERT b){
	return(ceil(distance(a,b)));
}