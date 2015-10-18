
#include "utils.h"

double distance(VERT a, VERT b){
	double x = a.x-b.x;
	double y = a.y-b.y;
	double z = a.z-b.z;
	return(sqrt(x*x+y*y+z*z));
}

unsigned int distance_ui(VERT a, VERT b){
	return((unsigned int)ceil(distance(a,b)));
}

//nerekurzivna verzia quicksortu P su prvky, s,e je rozsah, l je limit, pokial staci zotriedit
void qsortNR(unsigned int * P, int s, int e, int l){
 unsigned int * Fs =  new unsigned int[(e-s)/2 + 2];//zasobnik na triedene useky
 unsigned int * Fe =  new unsigned int[(e-s)/2 + 2];//zasobnik na triedene useky
 int Ftop = 1;
 unsigned int x,q;
 int z,k,i,j;
 
 Fs[1]=s;
 Fe[1]=e;

 while (Ftop){ //kym su nejake useky
	 z=Fs[Ftop];
	 k=Fe[Ftop--];
	 i=z;j=k;
	 x=P[i];

	 do{
		while(P[i] < x) i++;
		while(P[j] > x) j--;
		if (i<j){q=P[i];P[i]=P[j];P[j]=q;i++;j--;} else if (i==j) {i++;j--;}


	 }while (i<=j);
	 //usek <Z,K> je rozdelen....
	 if ((z<j)&&(z<l)) {Fs[++Ftop]=z;Fe[Ftop]=j;}
	 if ((i<k)&&(i<l)) {Fs[++Ftop]=i;Fe[Ftop]=k;}
 }

 delete[] Fs;
 delete[] Fe;
}