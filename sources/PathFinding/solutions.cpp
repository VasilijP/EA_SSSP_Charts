
#include <stdlib.h>
#include <limits.h>
#include "solutions.h"


SOLUTION* solveEmpty(const JOB * j);//empty implementation
SOLUTION* solveDijkstra(const JOB * j);//dijkstra shortest pathfinding algorithm
SOLUTION* solveBellmanFord(const JOB * j);//bellman ford shortest pathfinding algorithm

/*  Always update these fields together  */
/**/unsigned int ALGCOUNT = 3;								//number of algorithms in this unit
/**/
/**/char* AlgNames[] = {"empty","dijkstra","bellmanford"};				//array of names
/**/SOLUTION* (*(Algs[]))(const JOB *) = {solveEmpty,solveDijkstra,solveBellmanFord};	//array of func pointers (according to AlgNames)
/*****************************************/

SOLUTION* solveEmpty(const JOB * j){
	//for(int rpc = 0; rpc < 1000;rpc++);
	SOLUTION * rv = new SOLUTION;
	rv->exists = false;
	rv->l=0;
	rv->V=NULL;
	rv->vertNo=0;
	return(rv);	
}

//skusit este implementovat verziu kde fronta je jedna a dlha, Fp bude, ale vyhadzovat sa
// bude len tak ze sa prepise 0 a prida sa na koniec (tj fronta moze byt >> ako pocet vrcholov)
// mohlo by to byt rychlejsie ako prehadzovanie front a bude to viac
// kopirovat BF algoritmus, pretoze v tejto verzii sa zachova iba to ze nove pridane vrcholy
// sa skumaju v dalsej vrstve, ale nie nutne az po vsetkych co su tam uz pridane(to ako sa vyhadzuje v O(1)
// to sposobuje)
SOLUTION* solveBellmanFord(const JOB * j){
	int * Vs = new int[j->vertNo+1];	//index kde zacina zoznam nasledovnikov vo Vi
	int * Vc = new int[j->vertNo+1];	//pocet zaznamov vo Vi pre dany vrchol
	int * Vp = new int[j->vertNo+1];	//predchodcovia na ceste zo src
	int * Vi = new int[(j->edgeNo)*2];	//zoznam naslednikov, naslednici pre i zacinaju od Vi[Vs[i]]
	int * Vl = new int[(j->edgeNo)*2];	//dlzky hran

	int src = j->src;					//pociatok
	int dst = j->dst;					//koniec

	int cumc = 0;						//kumulativne cetnosti
	int hilimit = j->vertNo+1;	
	for (int rpc = 1; rpc< hilimit; rpc++ ) {
		Vs[rpc] = cumc;					//zoznam zacina v poli Vi na indexe cumc
		Vc[rpc] = 0;					//pocet naslednikov ulozeny pre kazdy vrchol je zatial 0
		cumc+=j->V[rpc].deg;			//posun o stupen vrchola
	}	

	int v1,v2;	
	hilimit=j->edgeNo;
	for (int rpc=0; rpc< hilimit; rpc++){
		EDGE e = j->E[rpc];
		v1 = e.v1;
		v2 = e.v2;
		Vl[Vs[v1]+Vc[v1]]=e.l;			//dlzka cesty
		Vi[Vs[v1]+Vc[v1]++]=v2;			//v1 prida do zoznamu naslednikov v2 a zvysi pocet vo Vc[v1]
		Vl[Vs[v2]+Vc[v2]]=e.l;
		Vi[Vs[v2]+Vc[v2]++]=v1;
	}

	int * Vd = new int[j->vertNo+1];	//vzdialenost do vrchola z source, INT_MAX pre neprebrany	
	int * F1 = new int[j->vertNo+1];	//indexuje sa od 1 po vertNo
	int * F1p = new int[j->vertNo+1];   //pozicia vrchola vo fronte, 0 = nieje tam
	int * F2 = new int[j->vertNo+1];    //indexuje sa od 1 po vertNo
	int * F2p = new int[j->vertNo+1];   //pozicia vrchola vo fronte, 0 = nieje tam
	hilimit=j->vertNo+1;
	for (int rpc=0;rpc<hilimit;rpc++) { F1p[rpc] = F2p[rpc] = 0; Vd[rpc] = INT_MAX; }
	F1[1] = src;						//najprv je tam len src
	F1p[src] = 1;						// a to na pozici 1 v F1
	Vd[src]=0;							//vzialenost do pociatku je 0
	int F1s = 1;						//ak je fronta neprazdna, index prveho prvku
	int F1e = 2;						//index prveho volneho miesta
	int F1c = 1;						//pocet prvkov vo fronte
	int F2s = 1;						//ak je fronta neprazdna, index prveho prvku
	int F2e = 1;						//index prveho volneho miesta
	int F2c = 0;						//pocet prvkov vo fronte
		
	int fvI;							//index prveho vrchola (vybraneho z fronty)
	while(F1c+F2c){						//kym je aspon v jednej fronte nejaky vrchol
		if (!F1c){						//uz sa minula prva fronta, prehodime tam druhu
			int * tmp = F1;	F1 = F2; F2 = tmp;
			tmp = F1p; F1p = F2p; F2p = tmp;
			F1s = F2s; F2s = 1;
			F1e = F2e; F2e = 1;
			F1c = F2c; F2c = 0;
		}
		
		fvI = F1[F1s]; F1s++; F1c--; F1p[fvI] = 0;
		hilimit = Vs[fvI]+Vc[fvI];
		for (int rpc = Vs[fvI]; rpc<hilimit; rpc++){//preberie nasledovnikov
			if (Vd[Vi[rpc]]>Vd[fvI]+Vl[rpc]){
				Vp[Vi[rpc]] = fvI;		//nastavi noveho predchodcu
				Vd[Vi[rpc]] = Vd[fvI]+Vl[rpc]; //nastavi novu vzdialenost
				if (F1p[Vi[rpc]]){		//ak je vo fronte, je najprv odobrany
					F1[Vi[rpc]] = F1[--F1e];F1c--;
					F1p[Vi[rpc]]=0;
				}
				if (!F2p[Vi[rpc]]){		//ak este nieje v druhej, je tam pridany
					F2[F2e] = Vi[rpc];  //prida na koniec fronty
					F2p[Vi[rpc]]=F2e;	//zapise mu jeho poziciu v F2
					F2e++; F2c++;		//zvysi pocet a koniec
				}
			}
		}
		//vybrat prvy z F1, odpisat ho z F1p...
		//pre naslednikov skusit zlepsenie
		//tam kde nastane, pridavat do F2, ale najprv odoberat z F1 ak treba (na to su F1p a F2p)

	}

	//vytvorenie vysledku
	SOLUTION * rv = new SOLUTION;
	if (Vd[dst]==INT_MAX) {							//ak to dobehlo bez dosiahnutia ciela
		rv->exists=false;
		rv->l=0;
		rv->V=NULL;
		rv->vertNo=0;		
	} else {
		//vytvorenie navratovej hodnoty v tvare: (dlzkacesty)src,v1,v2,...,vn,dst
		rv->exists = true;
		rv->l = Vd[dst];
		int i = dst,count = 1;
		do {
			i = Vp[i];
			count++;
		} while (i!=src);
		rv->vertNo = count;
		rv->V = new unsigned int[count];
		unsigned int * P = rv->V;	
		i = dst;
		P[--count] = dst;
		do {		
			i = Vp[i];
			P[--count] = i;
		} while (i!=src);
	}
	//uvolnovanie
	delete[] Vs;
	delete[] Vc;
	delete[] Vp;
	delete[] Vi;
	delete[] Vl;
	delete[] Vd;
	delete[] F1p;
	delete[] F2p;
	delete[] F1;
	delete[] F2;

	return(rv);
}

/**
Skusit dve dalsie verzie
1. pridat do fronty sort, mohlo by to velmi urychlit, pretoze pridane prvky na koniec by stacilo zatriedit
   a vyber minima by sa tym zrychlil
2. pridat BVS alebo nieco podobne, aby bolo pridanie aj vyber minima co najrychlejsi
*/
SOLUTION* solveDijkstra(const JOB * j){
	int * Vs = new int[j->vertNo+1];	//index kde zacina zoznam nasledovnikov vo Vi
	int * Vc = new int[j->vertNo+1];	//pocet zaznamov vo Vi pre dany vrchol
	int * Vp = new int[j->vertNo+1];	//predchodcovia na ceste zo src
	int * Vi = new int[(j->edgeNo)*2];	//zoznam naslednikov, naslednici pre i zacinaju od Vi[Vs[i]]
	int * Vl = new int[(j->edgeNo)*2];	//dlzky hran

	int src = j->src;					//pociatok
	int dst = j->dst;					//koniec

	int cumc = 0;						//kumulativne cetnosti
	int hilimit = j->vertNo+1;	
	for (int rpc = 1; rpc< hilimit; rpc++ ) {
		Vs[rpc] = cumc;					//zoznam zacina v poli Vi na indexe cumc
		Vc[rpc] = 0;					//pocet naslednikov ulozeny pre kazdy vrchol je zatial 0
		cumc+=j->V[rpc].deg;			//posun o stupen vrchola
	}	

	int v1,v2;	
	hilimit=j->edgeNo;
	for (int rpc=0; rpc< hilimit; rpc++){
		EDGE e = j->E[rpc];
		v1 = e.v1;
		v2 = e.v2;
		Vl[Vs[v1]+Vc[v1]]=e.l;			//dlzka cesty
		Vi[Vs[v1]+Vc[v1]++]=v2;			//v1 prida do zoznamu naslednikov v2 a zvysi pocet vo Vc[v1]
		Vl[Vs[v2]+Vc[v2]]=e.l;
		Vi[Vs[v2]+Vc[v2]++]=v1;
	}

	int * Vd =  new int[j->vertNo+1];	//vzdialenost do vrchola z source, INT_MAX pre neprebrany	
	int * F = new int[j->vertNo];
	F[0] = src;							//najprv je tam len src
	int Fs = 0;							//aj je fronta neprazdna, index prveho prvku
	int Fe = 1;							//index prveho volneho miesta
	int Fc = 1;							//pocet prvkov vo fronte

	hilimit=j->vertNo+1;
	for (int rpc = 1;rpc<hilimit;rpc++)	Vd[rpc] = INT_MAX;

	Vd[src]=0;

	int minI;							//index minima
	int vminI;							//index vrchola na prebranie
	while (Fc){							//kym nieje vrchol V[dst] prebrany a Fronta je neprazdna
		minI = Fs;
		for (int rpc = Fs+1; rpc < Fe; rpc++)
			minI = (Vd[F[minI]] < Vd[F[rpc]])? minI : rpc;//najde index minima		
		vminI = F[minI];
		if (vminI==dst) break;			//uz je preberany cielovy vrchol, takze sa lepsia cesta donho nenajde, mozme skoncit
		F[minI] = F[--Fe];
		Fc--;							//ak bol vminI == dst, skonci sa uz skor a vo fronte je aspon 1 prvok, podla toho sa rozlisi uspesne najdenie cesty
		
		hilimit = Vs[vminI]+Vc[vminI];
		for (int rpc=Vs[vminI]; rpc< hilimit; rpc++){			
			if (Vd[Vi[rpc]]==INT_MAX) {
				F[Fe++]=Vi[rpc];
				Fc++;
			}
			if (Vd[Vi[rpc]]>Vd[vminI]+Vl[rpc]){
				Vd[Vi[rpc]]=Vd[vminI]+Vl[rpc];
				Vp[Vi[rpc]]=vminI;
			}
		}
	}

	/*navrat riesenia*/
	SOLUTION * rv = new SOLUTION;
	if (!Fc) {							//ak to skoncilo vyprazdnenim fronty
		rv->exists=false;
		rv->l=0;
		rv->V=NULL;
		rv->vertNo=0;
	} else {
		//vytvorenie navratovej hodnoty v tvare: (dlzkacesty)src,v1,v2,...,vn,dst
		rv->exists = true;
		rv->l = Vd[dst];
		int i = dst,count = 1;
		do {
			i = Vp[i];
			count++;
		} while (i!=src);
		rv->vertNo = count;
		rv->V = new unsigned int[count];
		unsigned int * P = rv->V;	
		i = dst;
		P[--count] = dst;
		do {		
			i = Vp[i];
			P[--count] = i;
		} while (i!=src);
	}
	//uvolnovanie
	delete[] Vs;
	delete[] Vc;
	delete[] Vp;
	delete[] Vi;
	delete[] Vl;
	delete[] Vd;
	delete[] F;

	return(rv);
}