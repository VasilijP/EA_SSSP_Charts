//generator vstupov

#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "gen.h"
#include "List.h"
#include "utils.h"

#using<mscorlib.dll>

/*KONSTANTY pre generovanie grafu*/
//polomer priestoru v ktorom sa nachadzaju vrcholy
#define RNG 500
//vrcholy by mali byt indexovatelne 16 bitmi, takze najviac 64k, ale RAND_MAX je len 0x7FFF
#define MAXVERTS 30000

JOB*actualJob = NULL;//ukazatel na aktualne vygenerovanu ulohu
/*Udaje o poslednom generovani*/
unsigned int last_flags;
unsigned int last_vertNo;
unsigned int last_eLenAdd;
unsigned int last_conn;
unsigned int last_taskId;
/**/

//vrati nagenerovany job alebo NULL ak este nebolo generovanie spustene alebo bolo neuspesne
JOB*getJob(){
	return(actualJob);
}

//odstranuje aktualne nagenerovanu ulohu ak nieje actualJob == NULL
void deleteActualJob(){
	if (actualJob) {
		delete[] actualJob->E;
		delete[] actualJob->V;
		delete actualJob;
		actualJob = NULL;
	}
}

//kontroluje parametre, ak zodpovedaju korektnemu zadaniu vrati false, inak true
bool badParams(unsigned int flags,unsigned int vertNo, unsigned int eLenAdd, unsigned int conn, unsigned int taskId){		
	if ((flags & SPACE)&&(flags & FLAT)) return true;
	if (vertNo > MAXVERTS) return true;
	if ((flags & PEREDGECONN)&&(flags & MINEDGECONN)) return true;
	if ((flags & MINEDGECONN)&&(conn>=vertNo)) return true;
	if ((flags & PEREDGECONN)&&(conn>100)) return true;
	return(false);
}

//vygeneruje vrcholy, stupne nastavi na 0
void genVerts(unsigned int flags, unsigned int vertNo, JOB* j, System::Random * r){
	int x,y,z;
	unsigned int ZM = 0xffffffff;
	if (flags & FLAT) ZM = 0;	

	j->vertNo=0;//vrcholy maju cisla 1 az vertNo
	j->V = new VERT[vertNo+1];//o 1 viac, aby sa dalo indexovat 1..vertNo
	while((j->vertNo) < vertNo){			
		x=r->Next(-RNG,RNG+1);//vracia nahodne cislo z <-RNG,RNG> podla definicie Random.Next(int,int)
		y=r->Next(-RNG,RNG+1);
		z=r->Next(-RNG,RNG+1);
		z&=ZM;//vymaskovanie Z suradnice pri FLAT				
		if (sqrt((double)x*x+y*y+z*z)<=RNG){
			j->vertNo++;
			j->V[j->vertNo].deg=0;
		    j->V[j->vertNo].x=x;
			j->V[j->vertNo].y=y;
			j->V[j->vertNo].z=z;
		}		
	}
}

void genStartStop(JOB * j,System::Random * r){
	j->src = r->Next(1,j->vertNo+1);//vyberie src vrchol od 1 po vertNo		

	do { j->dst = r->Next(1,j->vertNo+1);//vyberie ciel rozny od startu
	} while (j->src==j->dst);

}

//vypocita dlzky hran
void genEdgesLen(JOB* j,unsigned int eLenAdd,System::Random* r){	
	double dlzka;	
	for (unsigned int rpc=0; rpc<j->edgeNo; rpc++){
		dlzka = distance(j->V[j->E[rpc].v1],j->V[j->E[rpc].v2]);
		dlzka *= 1 + 0.01*r->Next(0,eLenAdd+1);//predlzi o 0 az zadany pocet percent
		j->E[rpc].l = (unsigned int)ceil(dlzka);
	}
}

//vypocita stupne vrcholov
void genVertDegs(JOB * j){
	for (unsigned int rpc=0; rpc<j->vertNo; rpc++) j->V[rpc].deg = 0;
	for (unsigned int rpc=0; rpc<j->edgeNo; rpc++){
		j->V[j->E[rpc].v1].deg++;
		j->V[j->E[rpc].v2].deg++;
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
	*/
void genEdges(unsigned int flags, JOB* j, unsigned int conn, System::Random* r){
	List * l = new List();	

	//PEREDGECONN + NEAREST & MINEDGECONN + NEAREST
	if (flags&NEAREST){		
		unsigned int * vdegs = new unsigned int[j->vertNo+1];		
		if (flags&PEREDGECONN){
			double climit = 1/(1+sqrt(1-0.01*conn));
			for (unsigned int rpc=0; rpc<=j->vertNo; vdegs[rpc++]=0);
			for (unsigned int rpc1=1; rpc1<=j->vertNo; rpc1++)
				for (unsigned int rpc2=1; rpc2<j->vertNo; rpc2++)
					if (r->NextDouble() < climit) vdegs[rpc1]++;//nageneruje stupne podla pravdepodobnosti
		} else if(flags&MINEDGECONN) {
			for (unsigned int rpc=0; rpc<=j->vertNo; vdegs[rpc++]=conn);//vsade sa nastavi pevny pocet (aky bude minimalne stupen)
		} else assert(0);

		//pripojit ku najblizsim a vyhodit duplicity
		unsigned int v1;
		unsigned int * verts = new unsigned int[j->vertNo+1];
		for(unsigned int rpc1=1; rpc1<=j->vertNo; rpc1++){			
			for(unsigned int rpc2=1; rpc2<=j->vertNo; rpc2++)
				verts[rpc2]=(distance_ui(j->V[rpc1],j->V[rpc2])<<16)+rpc2;//horne dvojslovo bude vzdialenost, dolne cislo vrcholu
			verts[0]=0;verts[rpc1]=0;
			//qsort(verts,j->vertNo+1,sizeof(unsigned int),compare);//0. a 1. vynechame			
			qsortNR(verts,0,j->vertNo,vdegs[rpc1]+1);
			assert(verts[0]==0);assert(verts[1]==0);
			for(unsigned int rpc2=0;rpc2<vdegs[rpc1];rpc2++){
				v1 = verts[rpc2+2] & 0xFFFF;
				l->put((__min(rpc1,v1)<<16)+__max(rpc1,v1));
			}
		}		
		l->dupkick();
		delete[] vdegs;
		delete[] verts;
	} else {//PEREDGECONN & MINEDGECONN bez NEAREST

		if (flags&PEREDGECONN){
			double climit = conn*0.01;
			for (unsigned int rpc1=1; rpc1<j->vertNo; rpc1++)
				for (unsigned int rpc2=rpc1+1; rpc2<=j->vertNo; rpc2++)
					if (r->NextDouble() < climit) l->put((rpc1 << 16) + rpc2);//horne dvojslovo je jeden index, dolne druhy index
		} else if(flags&MINEDGECONN){
			//vygenerovat pre kazdy vrchol conn roznych hran a vyhodit duplicity
			bool * vb = new bool[j->vertNo+1];			
			unsigned int cnt;
			unsigned int vertI;
			for (unsigned int vert = 1; vert<=j->vertNo; vert++){
				for(unsigned int rpc=1; rpc<=j->vertNo; vb[rpc++]=true);
				vb[vert]=false;//nechceme slucky v grafe
				cnt=conn;
				while (cnt){
					vertI = r->Next(1,j->vertNo+1);
					if (vb[vertI]){
						vb[vertI]=false;
						cnt--;
						l->put((__min(vert,vertI)<<16)+__max(vert,vertI));
					}
				}
			}
			l->dupkick();//vyhodi duplicity
			delete[] vb;
		} else assert(0);		
	}

	//vytvorenie hran z Listu
	j->edgeNo = l->getSize();
	j->E = new EDGE[j->edgeNo];
	unsigned int vertCode;
	for (unsigned int rpc = 0; rpc<j->edgeNo; rpc++){
		vertCode = l->get();
		j->E[rpc].v1 = (vertCode >> 16) & 0xFFFF;
		j->E[rpc].v2 = vertCode & 0xFFFF;
	}
}

//true
bool sameJob(unsigned int flags,unsigned int vertNo, unsigned int eLenAdd, unsigned int conn, unsigned int taskId){
	if (!actualJob) return false;
	if (last_flags!=flags) return false;
    if (last_vertNo!= vertNo) return false;
	if (last_eLenAdd!= eLenAdd) return false;
	if (last_conn != conn) return false;
	if (last_taskId != taskId) return false;
	return true;
}

//generuje job podla parametrov
//flags obsahuje kombinacie SPACE, FLAT, NEAREST, PEREDGECONN, MINEDGECONN
//vylucuju sa (SPACE a FLAT) a (PEREDGECONN a MINEDGECONN)
// pre PEREDGECONN v conn1 je hodnota v %, pre MINEDGECONN je to minimum # susedov pre kazdy vrchol
//eLenAdd je hodnota v % popisana v readme (predlzovanie hrany)
void genJob(unsigned int flags,unsigned int vertNo, unsigned int eLenAdd, unsigned int conn, unsigned int taskId){				

	if (actualJob && sameJob(flags,vertNo,eLenAdd,conn,taskId)) return;//ak uz je uloha pripravena, negeneruje sa znovu
	last_flags = flags;
	last_vertNo = vertNo;
	last_eLenAdd = eLenAdd;
	last_conn = conn;
	last_taskId = taskId;

	deleteActualJob();	//zmaze nagenerovanu ulohu a nastavi actualJob na NULL

	if (badParams(flags,vertNo,eLenAdd,conn,taskId)) return; //skonci a v actualJob necha NULL		

	//vytvorenie struktury
	JOB* j = new JOB;	
	j->taskId = taskId;//zapisanie taskId
	
	System::Random * random = new System::Random((int)taskId);//priprava generatora

	//generovanie vrcholov
	genVerts(flags,vertNo,j,random);

	//generovanie hran
	genEdges(flags,j,conn,random);

	//dlzky hran
	genEdgesLen(j,eLenAdd,random);

	//stupne vrcholov
	genVertDegs(j);
	
	//start a ciel
	genStartStop(j,random);	

	//return
	actualJob = j;		
}

void genJob(unsigned int flags,unsigned int vertNo, unsigned int eLenAdd, unsigned int conn){
	genJob(flags, vertNo, eLenAdd, conn, (unsigned int)time( NULL ));
}
