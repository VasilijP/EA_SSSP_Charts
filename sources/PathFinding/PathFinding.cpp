// PathFinding.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include "gen.h"
#include "types.h"

int _tmain(int argc, _TCHAR* argv[])
{	

	genJob(MINEDGECONN | NEAREST | FLAT,1000,25,10,50);
	JOB * j = getJob();
	
	return 0;
}