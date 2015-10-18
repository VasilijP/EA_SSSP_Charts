
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "utils.h"
#include "solutions.h"


SOLUTION* solveEmpty(const JOB * j);//empty implementation
SOLUTION* solveBellmanFord(const JOB * j);//bellman ford shortest pathfinding algorithm
SOLUTION* solveBellmanFordLimit(const JOB * j);//bellman ford s upravou (orezanie prehladavania podla doposial najdenej cesty)
SOLUTION* solveBellmanFordGeom(const JOB * j);//bellman ford s upravou (orezanie prehladavania podla geometrickych informacii)
SOLUTION* solveDijkstra(const JOB * j);//dijkstra shortest pathfinding algorithm
SOLUTION* solveDijkstraHeap(const JOB * j);//dijkstra shortest pathfinding algorithm + heap
SOLUTION* solveDijkstraHeapGeom(const JOB * j);//dijkstra shortest pathfinding algorithm + heap + use of geometric informations


/*  Always update these fields together  */
/**/unsigned int ALGCOUNT = 7;								//number of algorithms in this unit
/**/
/**/char* AlgNames[] = {"empty",
						"dijkstra",
						"bellmanford",
						"dijkstraheap",
						"bellmanfordlimit",
						"dijkstraheapgeom",
						"bellmanfordgeom"};//array of names
/**/SOLUTION* (*(Algs[]))(const JOB *) = {solveEmpty,
										  solveDijkstra,
										  solveBellmanFord,
										  solveDijkstraHeap,
										  solveBellmanFordLimit,
										  solveDijkstraHeapGeom,
										  solveBellmanFordGeom
										};	//array of func pointers (according to AlgNames)
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

SOLUTION* solveDijkstraHeap(const JOB * j){
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
	int * H = new int[j->vertNo+1];		//indexuje sa od 1, 0 v Hp = ze tam nieje
	int * Hp = new int[j->vertNo+1];	//pozicie vrcholov v heape
	H[1] = src;							//najprv je tam len src
	hilimit = j->vertNo+1;
	for (int rpc=0; rpc<hilimit;rpc++){ Vd[rpc] = INT_MAX; Hp[rpc]=0; }
	Hp[src] = 1;
	int Hc = 1;							//pocet hodnot v heape			

	Vd[src]=0;
	
	int vminI;							//index vrchola na prebranie
	while (Hc){							//kym nieje vrchol V[dst] prebrany a Fronta je neprazdna
		/*vyber minima z heapu*/
		vminI = H[1];
		Hp[vminI] = 0;				
		/**/
		if (vminI==dst) break;			//uz je preberany cielovy vrchol, takze sa lepsia cesta donho nenajde, mozme skoncit		
		//oprava struktury heapu		
		Hc--;							//ak bol vminI == dst, skonci sa uz skor a v Fc je aspon 1, podla toho sa rozlisi ci sa nasla cesta
		if (Hc){
			int X = H[Hc+1];
			int j = 1;
			int n;
			bool pokracovat = 2<=Hc;
			while (pokracovat) {
				n=2*j;
				n+=((n<Hc)&&(Vd[H[n+1]]<Vd[H[n]]));
				if (Vd[X]>Vd[H[n]]){
					H[j] = H[n];
					Hp[H[j]] = j;
					j=n;
					pokracovat = 2*j <= Hc;
				} else pokracovat = false;							
			}
			H[j] = X;
			Hp[X] = j;
		}

		hilimit = Vs[vminI]+Vc[vminI];
		for (int rpc=Vs[vminI]; rpc< hilimit; rpc++)
			if (Vd[Vi[rpc]]>Vd[vminI]+Vl[rpc]){
				Vd[Vi[rpc]]=Vd[vminI]+Vl[rpc];
				Vp[Vi[rpc]]=vminI;
				//pridat alebo upravit poziciu Vi[rpc] v heape pretoze sa zmenila vzdialenost
				int j = Hp[Vi[rpc]];
				int p;
				int X = Vi[rpc];
				if (!j)//ak tam nie je treba ho pridat
				 j = ++Hc;
				bool pokracovat = j > 1;
				while (pokracovat){
					p = j / 2;
					if (Vd[X] < Vd[H[p]]){
						H[j] = H[p];
						Hp[H[j]] = j;
						j=p;
						pokracovat = j>1;
					} else pokracovat = false;										
				}				
				H[j] = X;
				Hp[X] = j;
			}		
	}

	/*navrat riesenia*/
	SOLUTION * rv = new SOLUTION;
	if (!Hc) {							//ak to skoncilo vyprazdnenim fronty
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
	delete[] H;
	delete[] Hp;

	return(rv);
}

SOLUTION* solveDijkstraHeapGeom(const JOB * j){//s vyuzitim geometrickych udajov
	int * Vs = new int[j->vertNo+1];	//index kde zacina zoznam nasledovnikov vo Vi
	int * Vc = new int[j->vertNo+1];	//pocet zaznamov vo Vi pre dany vrchol
	int * Vp = new int[j->vertNo+1];	//predchodcovia na ceste zo src
	int * Vm = new int[j->vertNo+1];	//minimalna dlzka cesty spocitana ako eukl. vzdialenost
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
	int dx=j->V[j->dst].x, dy=j->V[j->dst].y, dz=j->V[j->dst].z;
	int rx,ry,rz;
	for (int rpc = 1; rpc<hilimit; rpc++){		//spocitanie minimalnych vzdialenosti
		rx = j->V[rpc].x - dx;
		ry = j->V[rpc].y - dy;
		rz = j->V[rpc].z - dz;
		Vm[rpc] = int(floor(sqrt(double(rx*rx+ry*ry+rz*rz))));	//ulozene ako druha odmocnina
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
	int * H = new int[j->vertNo+1];		//indexuje sa od 1, 0 v Hp = ze tam nieje
	int * Hp = new int[j->vertNo+1];	//pozicie vrcholov v heape
	H[1] = src;							//najprv je tam len src
	hilimit = j->vertNo+1;
	for (int rpc=0; rpc<hilimit;rpc++){ Vd[rpc] = INT_MAX; Hp[rpc]=0; }
	Hp[src] = 1;
	int Hc = 1;							//pocet hodnot v heape			

	Vd[src]=0;
	
	int vminI;							//index vrchola na prebranie
	while (Hc){							//kym nieje vrchol V[dst] prebrany a Fronta je neprazdna
		/*vyber minima z heapu*/
		vminI = H[1];
		Hp[vminI] = 0;				
		/**/
		if (vminI==dst) break;			//uz je preberany cielovy vrchol, takze sa lepsia cesta donho nenajde, mozme skoncit		
		//oprava struktury heapu		
		Hc--;							//ak bol vminI == dst, skonci sa uz skor a v Fc je aspon 1, podla toho sa rozlisi ci sa nasla cesta
		if (Hc){
			int X = H[Hc+1];
			int j = 1;
			int n;
			bool pokracovat = 2<=Hc;
			while (pokracovat) {
				n=2*j;
				n+=((n<Hc)&&(Vd[H[n+1]]<Vd[H[n]]));
				if (Vd[X]>Vd[H[n]]){
					H[j] = H[n];
					Hp[H[j]] = j;
					j=n;
					pokracovat = 2*j <= Hc;
				} else pokracovat = false;							
			}
			H[j] = X;
			Hp[X] = j;
		}
		if (Vd[vminI]+Vm[vminI] > Vd[dst]) continue;//ak je vzdialenost vrchola + minimalna dlzka cesty do ciela vacsia ako sucasna, vynechame vrchol

		hilimit = Vs[vminI]+Vc[vminI];
		for (int rpc=Vs[vminI]; rpc< hilimit; rpc++)
			if (Vd[Vi[rpc]]>Vd[vminI]+Vl[rpc]){
				Vd[Vi[rpc]]=Vd[vminI]+Vl[rpc];
				Vp[Vi[rpc]]=vminI;
				//pridat alebo upravit poziciu Vi[rpc] v heape pretoze sa zmenila vzdialenost
				int j = Hp[Vi[rpc]];
				int p;
				int X = Vi[rpc];
				if (!j)//ak tam nie je treba ho pridat
				 j = ++Hc;
				bool pokracovat = j > 1;
				while (pokracovat){
					p = j / 2;
					if (Vd[X] < Vd[H[p]]){
						H[j] = H[p];
						Hp[H[j]] = j;
						j=p;
						pokracovat = j>1;
					} else pokracovat = false;										
				}				
				H[j] = X;
				Hp[X] = j;
			}		
	}

	/*navrat riesenia*/
	SOLUTION * rv = new SOLUTION;
	if (!Hc) {							//ak to skoncilo vyprazdnenim fronty
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
	delete[] Vm;
	delete[] Vd;
	delete[] H;
	delete[] Hp;

	return(rv);
}

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
	int Fsize = 4*(j->vertNo+1);		//pociatocna velkost fronty
	int * F = new int[Fsize];			//indexuje sa od 1
	int * Fp = new int[j->vertNo+1];    //pozicia vrchola vo fronte, 0 = nieje tam	
	hilimit=j->vertNo+1;
	for (int rpc=0;rpc<hilimit;rpc++) { Fp[rpc] = 0; Vd[rpc] = INT_MAX; }
	F[1] = src;							//najprv je tam len src
	Fp[src] = 1;						// a to na pozici 1 v F1
	Vd[src]=0;							//vzialenost do pociatku je 0
	int Fs = 1;							//ak je fronta neprazdna, index prveho prvku
	int Fe = 2;							//index prveho volneho miesta
	int Fc = 1;							//pocet prvkov vo fronte	

	int fvI;							//index prveho vrchola (vybraneho z fronty)
	while(Fc){							//kym je vo fronte nejaky vrchol				

		do {fvI = F[Fs]; Fs=(Fs % (Fsize-1))+1; Fc--; Fp[fvI] = 0;//zmena indexu (Fs % (Fsize-1))+1 je ok,pretoze sa indexuje od 1 po Fsize-1
		}while ((Fc>0)&&(fvI==0));//0 su prazdne miesta
		if (fvI==0) break;//uz tam nieje nic
		hilimit = Vs[fvI]+Vc[fvI];
		for (int rpc = Vs[fvI]; rpc<hilimit; rpc++){//preberie nasledovnikov
			if (Vd[Vi[rpc]]>Vd[fvI]+Vl[rpc]){
				Vp[Vi[rpc]] = fvI;		//nastavi noveho predchodcu
				Vd[Vi[rpc]] = Vd[fvI]+Vl[rpc]; //nastavi novu vzdialenost				
				if (Fp[Vi[rpc]])		//ak je vo fronte, je najprv prepisany nulou
					F[Fp[Vi[rpc]]] = 0;														
				if (Fc == Fsize-1){		//uz by do fronty nevosiel
					//prealokovat frontu
					int * tmp = new int[Fsize*2];//raz tolko
					int hilimit=__min(Fsize-Fs,Fc);
					for(int rpc=0; rpc<hilimit; rpc++) tmp[rpc+1]=F[Fs+rpc];//skopiruje od Fs po koniec pola
					int hilimit2 = Fc-hilimit;//zostava skopirovat Fc - hilimit
					for(int rpc=0; rpc<hilimit2; rpc++) tmp[rpc+hilimit+1]=F[1+rpc];//skopiruje zvysok
					Fs = 1;
					Fe = Fc+1;
					delete[] F;
					Fsize *= 2;
					F = tmp;
					hilimit = Fc+1;
					for(int rpc=1; rpc<hilimit; rpc++) Fp[F[rpc]]=rpc;//prepise zaradenie
				}
				F[Fe] = Vi[rpc];//prida nakoniec
				Fp[Vi[rpc]] = Fe;//zapise kam ho zaradil
				Fe=(Fe % (Fsize-1))+1;//posunie koniec
				Fc++;//zvysi pocet zaznamov vo fronte
			}
		}		
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
	delete[] Fp;
	delete[] F;	

	return(rv);
}

SOLUTION* solveBellmanFordLimit(const JOB * j){
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
	for (int rpc=0; rpc < hilimit; rpc++){
		EDGE e = j->E[rpc];
		v1 = e.v1;
		v2 = e.v2;
		Vl[Vs[v1]+Vc[v1]]=e.l;			//dlzka cesty
		Vi[Vs[v1]+Vc[v1]++]=v2;			//v1 prida do zoznamu naslednikov v2 a zvysi pocet vo Vc[v1]
		Vl[Vs[v2]+Vc[v2]]=e.l;
		Vi[Vs[v2]+Vc[v2]++]=v1;
	}

	int * Vd = new int[j->vertNo+1];	//vzdialenost do vrchola z source, INT_MAX pre neprebrany	
	int Fsize = 4*(j->vertNo+1);		//pociatocna velkost fronty
	int * F = new int[Fsize];			//indexuje sa od 1
	int * Fp = new int[j->vertNo+1];    //pozicia vrchola vo fronte, 0 = nieje tam	
	hilimit=j->vertNo+1;
	for (int rpc=0;rpc<hilimit;rpc++) { Fp[rpc] = 0; Vd[rpc] = INT_MAX; }
	F[1] = src;							//najprv je tam len src
	Fp[src] = 1;						// a to na pozici 1 v F1
	Vd[src]=0;							//vzialenost do pociatku je 0
	int Fs = 1;							//ak je fronta neprazdna, index prveho prvku
	int Fe = 2;							//index prveho volneho miesta
	int Fc = 1;							//pocet prvkov vo fronte	

	int fvI;							//index prveho vrchola (vybraneho z fronty)
	while(Fc){							//kym je vo fronte nejaky vrchol				

		do {fvI = F[Fs]; Fs=(Fs % (Fsize-1))+1; Fc--; Fp[fvI] = 0;//zmena indexu (Fs % (Fsize-1))+1 je ok,pretoze sa indexuje od 1 po Fsize-1
		}while ( Fc &&(!fvI));//0 su prazdne miesta
		if (fvI==0) break;//uz tam nieje nic
		if (Vd[fvI] > Vd[dst]) continue;//pokracuje ak teraz cestou cez tento vrchol nemoze zlepsit
		hilimit = Vs[fvI]+Vc[fvI];
		for (int rpc = Vs[fvI]; rpc<hilimit; rpc++){//preberie nasledovnikov
			if (Vd[Vi[rpc]]>Vd[fvI]+Vl[rpc]){
				Vp[Vi[rpc]] = fvI;		//nastavi noveho predchodcu
				Vd[Vi[rpc]] = Vd[fvI]+Vl[rpc]; //nastavi novu vzdialenost				
				if (Fp[Vi[rpc]])		//ak je vo fronte, je najprv prepisany nulou
					F[Fp[Vi[rpc]]] = 0;														
				if (Fc == Fsize-1){		//uz by do fronty nevosiel
					//prealokovat frontu
					int * tmp = new int[Fsize*2];//raz tolko
					int hilimit=__min(Fsize-Fs,Fc);
					for(int rpc=0; rpc<hilimit; rpc++) tmp[rpc+1]=F[Fs+rpc];//skopiruje od Fs po koniec pola
					int hilimit2 = Fc-hilimit;//zostava skopirovat Fc - hilimit
					for(int rpc=0; rpc<hilimit2; rpc++) tmp[rpc+hilimit+1]=F[1+rpc];//skopiruje zvysok
					Fs = 1;
					Fe = Fc+1;
					delete[] F;
					Fsize *= 2;
					F = tmp;
					hilimit = Fc+1;
					for(int rpc=1; rpc<hilimit; rpc++) Fp[F[rpc]]=rpc;//prepise zaradenie
				}
				F[Fe] = Vi[rpc];//prida nakoniec
				Fp[Vi[rpc]] = Fe;//zapise kam ho zaradil
				Fe=(Fe % (Fsize-1))+1;//posunie koniec
				Fc++;//zvysi pocet zaznamov vo fronte
			}
		}		
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
	delete[] Fp;
	delete[] F;	

	return(rv);
}

SOLUTION* solveBellmanFordGeom(const JOB * j){
	int * Vs = new int[j->vertNo+1];	//index kde zacina zoznam nasledovnikov vo Vi
	int * Vc = new int[j->vertNo+1];	//pocet zaznamov vo Vi pre dany vrchol
	int * Vp = new int[j->vertNo+1];	//predchodcovia na ceste zo src
	int * Vm = new int[j->vertNo+1];	//minimalna dlzka cesty spocitana ako eukl. vzdialenost
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
	int dx=j->V[j->dst].x, dy=j->V[j->dst].y, dz=j->V[j->dst].z;
	int rx,ry,rz;
	for (int rpc = 1; rpc<hilimit; rpc++){		//spocitanie minimalnych vzdialenosti
		rx = j->V[rpc].x - dx;
		ry = j->V[rpc].y - dy;
		rz = j->V[rpc].z - dz;
		Vm[rpc] = int(floor(sqrt(double(rx*rx+ry*ry+rz*rz))));
	}

	int v1,v2;	
	hilimit=j->edgeNo;
	for (int rpc=0; rpc < hilimit; rpc++){
		EDGE e = j->E[rpc];
		v1 = e.v1;
		v2 = e.v2;
		Vl[Vs[v1]+Vc[v1]]=e.l;			//dlzka cesty
		Vi[Vs[v1]+Vc[v1]++]=v2;			//v1 prida do zoznamu naslednikov v2 a zvysi pocet vo Vc[v1]
		Vl[Vs[v2]+Vc[v2]]=e.l;
		Vi[Vs[v2]+Vc[v2]++]=v1;
	}	

	int * Vd = new int[j->vertNo+1];	//vzdialenost do vrchola z source, INT_MAX pre neprebrany	
	int Fsize = 4*(j->vertNo+1);		//pociatocna velkost fronty
	int * F = new int[Fsize];			//indexuje sa od 1
	int * Fp = new int[j->vertNo+1];    //pozicia vrchola vo fronte, 0 = nieje tam	
	hilimit=j->vertNo+1;
	for (int rpc=0;rpc<hilimit;rpc++) { Fp[rpc] = 0; Vd[rpc] = INT_MAX; }
	F[1] = src;							//najprv je tam len src
	Fp[src] = 1;						// a to na pozici 1 v F1
	Vd[src]=0;							//vzialenost do pociatku je 0
	int Fs = 1;							//ak je fronta neprazdna, index prveho prvku
	int Fe = 2;							//index prveho volneho miesta
	int Fc = 1;							//pocet prvkov vo fronte	

	int fvI;							//index prveho vrchola (vybraneho z fronty)
	while(Fc){							//kym je vo fronte nejaky vrchol				

		do {fvI = F[Fs]; Fs=(Fs % (Fsize-1))+1; Fc--; Fp[fvI] = 0;//zmena indexu (Fs % (Fsize-1))+1 je ok,pretoze sa indexuje od 1 po Fsize-1
		}while ( Fc &&(!fvI));//0 su prazdne miesta
		if (fvI==0) break;//uz tam nieje nic
		if (Vd[fvI] + Vm[fvI] > Vd[dst]) continue;//pokracuje ak teraz cestou cez tento vrchol nemoze zlepsit
		hilimit = Vs[fvI]+Vc[fvI];
		for (int rpc = Vs[fvI]; rpc<hilimit; rpc++){//preberie nasledovnikov
			if (Vd[Vi[rpc]]>Vd[fvI]+Vl[rpc]){
				Vp[Vi[rpc]] = fvI;		//nastavi noveho predchodcu
				Vd[Vi[rpc]] = Vd[fvI]+Vl[rpc]; //nastavi novu vzdialenost				
				if (Fp[Vi[rpc]])		//ak je vo fronte, je najprv prepisany nulou
					F[Fp[Vi[rpc]]] = 0;														
				if (Fc == Fsize-1){		//uz by do fronty nevosiel
					//prealokovat frontu
					int * tmp = new int[Fsize*2];//raz tolko
					int hilimit=__min(Fsize-Fs,Fc);
					for(int rpc=0; rpc<hilimit; rpc++) tmp[rpc+1]=F[Fs+rpc];//skopiruje od Fs po koniec pola
					int hilimit2 = Fc-hilimit;//zostava skopirovat Fc - hilimit
					for(int rpc=0; rpc<hilimit2; rpc++) tmp[rpc+hilimit+1]=F[1+rpc];//skopiruje zvysok
					Fs = 1;
					Fe = Fc+1;
					delete[] F;
					Fsize *= 2;
					F = tmp;
					hilimit = Fc+1;
					for(int rpc=1; rpc<hilimit; rpc++) Fp[F[rpc]]=rpc;//prepise zaradenie
				}
				F[Fe] = Vi[rpc];//prida nakoniec
				Fp[Vi[rpc]] = Fe;//zapise kam ho zaradil
				Fe=(Fe % (Fsize-1))+1;//posunie koniec
				Fc++;//zvysi pocet zaznamov vo fronte
			}
		}		
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
	delete[] Vm;	
	delete[] Vl;
	delete[] Vd;
	delete[] Fp;
	delete[] F;	

	return(rv);
}
