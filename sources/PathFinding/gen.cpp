//generator vstupov

#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "gen.h"
#include "List.h"
#include "utils.h"

/*KONSTANTY pre generovanie grafu*/
//polomer priestoru v ktorom sa nachadzaju vrcholy
#define RNG 500
//vrcholy by mali byt indexovatelne 16 bitmi, takze najviac 64k, ale RAND_MAX je len 0x7FFF
#define MAXVERTS 30000

JOB*actualJob = NULL;

//vrati nagenerovany job
JOB*getJob(){
	return(actualJob);
}

//generuje job podla parametrov
//flags obsahuje kombinacie SPACE, FLAT, NEAREST, PEREDGECONN, MINEDGECONN
//vylucuju sa (SPACE a FLAT) a (PEREDGECONN a MINEDGECONN)
// pre PEREDGECONN v conn1 je hodnota v %, pre MINEDGECONN je to minimum # susedov pre kazdy vrchol
//eLenAdd je hodnota v % popisana v readme (predlzovanie hrany)
void genJob(unsigned int flags,unsigned int vertNo, unsigned int eLenAdd, unsigned int conn, unsigned int taskId){				
	unsigned int ZM = 0xffffffff;
	if (flags & FLAT) ZM = 0;
	if (((flags & SPACE)&&(ZM==0))||
		((flags & PEREDGECONN)&&(flags & MINEDGECONN))||
		((flags & MINEDGECONN)&&(conn>=vertNo))||
		((vertNo > MAXVERTS))) {
		 actualJob = NULL;
	     return;
		}
	
	int x,y,z;

	JOB* j = new JOB;
	j->vertNo=0;//vrcholy maju cisla 1 az vertNo
	j->V = new VERT[vertNo+1];//o 1 viac, aby sa dalo indexovat 1..vertNo
	j->taskId = taskId;
	srand(j->taskId);
	while((j->vertNo) < vertNo){			
		x=int(((rand()*2.0)/RAND_MAX)*RNG - RNG);
		y=int(((rand()*2.0)/RAND_MAX)*RNG - RNG);
		z=int(((rand()*2.0)/RAND_MAX)*RNG - RNG);
		z&=ZM;//vymaskovanie Z suradnice pri FLAT				
		if (sqrt((double)x*x+y*y+z*z)<=RNG){
			j->vertNo++;
			j->V[j->vertNo].deg=0;
		    j->V[j->vertNo].x=x;
			j->V[j->vertNo].y=y;
			j->V[j->vertNo].z=z;
		}		
	}

	/*
	PEREDGECONN
	 - prechadzaju sa mozne hrany a pre kazdu je hodene kockou, ma sancu conn v % ze
	   bude pridana...
	PEREDGECONN + NEAREST
	 - prejdu sa vsetky vrcholy a k nim sa vertNo krat s pravdepodobnostou
	   1/(1+sqrt(1-conn)) pripocita +1 (tato udalost je
	   v zlozenej pravdepodobnosti ekvivalentna k zaradeniu hrany!!!, 
	   ak oznacime 1/(1+sqrt(1-conn)) = a, potom a^2+2a*(1-a) = conn, to je sucet 
	   pravdepodobnosti ze bude zaradena dvojica [i,j], [j,i] alebo obe sucasne
     - kazdy vrchol je pripojeny k tolkym najblizsim vrcholom kolko 
	   krat sa k nemu zapocitala +1
     - duplicitne hrany su odobrane	
	MINEDGECONN
	 - pre kazdy vrchol sa vygeneruje conn hran obsahujucich tento vrchol, takze
	   kazdy vrchol bude mat minimalne conn susedov
	 - duplicitne hrany su odobrane
	MINEDGECONN + NEAREST
	 - vyberie sa pre kazdy vrchol prave conn najblizsich vrcholov
	 - duplicitne hrany su odobrane

	 napokon je pre kazdu hranu vygenerovana dlzka a su vypocitane stupne vrcholov
	*/

	List * l = new List();
	unsigned int min, max;

	//PEREDGECONN + NEAREST & MINEDGECONN + NEAREST
	if (flags&NEAREST){		
		unsigned int * vdegs = new unsigned int[vertNo+1];
		unsigned int * verts = new unsigned int[vertNo+1];
		if (flags&PEREDGECONN){
			unsigned int climit = unsigned int(RAND_MAX*(0.01*conn/(1+sqrt(1-0.01*conn))))+1;
			for (min=0; min<=vertNo;vdegs[min++]=0);
			for (min=1; min<=vertNo; min++)
				for (max=1; max<vertNo; max++)
					if (rand()<climit) vdegs[min]++;
		} else if(flags&MINEDGECONN) {
			for (min=0; min<=vertNo;vdegs[min++]=conn);
		} else assert(0);

		//pripojit ku najblizsim a vyhodit duplicity
		unsigned int v1;
		for(min=1; min<=vertNo;min++){			
			for(max=1; max<=vertNo; max++)
				verts[max]=(distance_ui(j->V[min],j->V[max])<<16)+max;
			verts[0]=0;verts[min]=0;
			qsort(verts,vertNo+1,sizeof(unsigned int),compare);//0. a 1. vynechame			
			assert(verts[0]==0);assert(verts[1]==0);
			for(max=0;max<vdegs[min];max++){
				v1 = verts[max+2] & 0xFFFF;				
				l->put((__min(min,v1)<<16)+__max(min,v1));
			}
		}		
		l->dupkick();

	} else {//PEREDGECONN & MINEDGECONN

		if (flags&PEREDGECONN){
			unsigned int climit = unsigned int(RAND_MAX*(conn*0.01))+1;
			for (min=1; min<vertNo; min++)
				for (max=min+1; max<=vertNo; max++)
					if (rand()<climit) l->put((min << 16) + max);
		} else if(flags&MINEDGECONN){
			//vygenerovat pre kazdy vrchol conn roznych hran a vyhodit duplicity
			bool * vb = new bool[vertNo+1];
			vb[0]=false;
			unsigned int cnt;
			for (min = 1; min<=vertNo; min++){
				for(max=1; max<=vertNo; vb[max++]=true);
				vb[min]=false;
				cnt=conn;
				while (cnt){
					max = rand()*vertNo/RAND_MAX;
					if (vb[max]){
						vb[max]=false;
						cnt--;
						l->put((__min(max,min)<<16)+__max(max,min));
					}
				}
			}
			l->dupkick();
		} else assert(0);
		
	}

	//vytvorenie hran z Listu
	j->edgeNo = l->getSize();
	j->E = new EDGE[j->edgeNo];
	for (min = 0; min<j->edgeNo; min++){
		max = l->get();
		j->E[min].v1 = (max >> 16) & 0xFFFF;
		j->E[min].v2 = max & 0xFFFF;
	}

	//dlzky hran	
	unsigned int rpc;
	double lenAdd = eLenAdd*0.01;
	double dlzka;
	for (rpc=0; rpc<j->edgeNo; rpc++){
		dlzka = distance(j->V[j->E[rpc].v1],j->V[j->E[rpc].v2]);
		dlzka *= 1 + (rand()*lenAdd)/RAND_MAX;
		j->E[rpc].l = (unsigned int)ceil(dlzka);
		j->V[j->E[rpc].v1].deg++;
		j->V[j->E[rpc].v2].deg++;
	}
	//start a ciel
	j->src = 1 + rand()*(vertNo-1)/RAND_MAX;
	do {
		j->dst = 1 + rand()*(vertNo-1)/RAND_MAX;
	} while (j->src==j->dst);
	

	//return
	actualJob= j;
}

void genJob(unsigned int flags,unsigned int vertNo, unsigned int eLenAdd, unsigned int conn){
	genJob(flags, vertNo, eLenAdd, conn, (unsigned int)time( NULL ));
}
