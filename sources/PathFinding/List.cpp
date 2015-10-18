
#include <assert.h>
#include <stdlib.h>
#include "list.h"
#include "utils.h"


List::List(void){
	f = NULL;//uloziste prvkov
	Size = 0;//alokovana velkost
	Count = 0;//pocet poloziek
	Start = 0;//index prveho
	End = 0;//index prveho volneho miesta
}

List::~List(void){
	if (f!=NULL) delete[] f;
}

bool List::empty(void){
	return(Count==0);
}

//zisti ci je prvok v zozname
bool List::contains(unsigned int o){
	if (Size == 0)
		return (false);

	bool lResult = false;
	unsigned int rpc;
	for (rpc = 0; rpc < Count; rpc++) {
		lResult = lResult || (f[(Start + rpc) % Size] == o);
	}
	return (lResult);
}

//vlozi prvok na koniec
void List::put(unsigned int v){
	if (Count >= Size) { //ak pocet vlozenych prvkov dosiahol velkost fronty a my vkladame dalsi prvok 
		unsigned int * tmp = new unsigned int[2 * (Size + 1)]; //naalokuje raz tolko		
		End = 0; //koniec fronty v novom priestore nastavi na 0
		while ((tmp[End++] = get()) != 0);//kopiruje Objecty do noveho pola, pritom ich vlastne odobera z fronty
		End--; //ked skonci je posledna vlozena hodnota 0, takze je End nastaveny na volne miesto
		Count = End; //ak vsetko prebehlo korektne, skopirovali sa prave vsetky prvky
		Start = 0; //to je ok, start ukazuje na prvy prvok
		Size = 2 * (Size + 1); //velkost je uz teraz 2*(povodna velkost + 1 )
		delete[] f;
		f = tmp; //napokon nahradime referenciu na staru frontu tou ktoru sme pripravili
	}
	f[End] = v; //standardne vlozenie prvku na koniec
	End = (End + 1) % Size;//prepocitanie noveho indexu konca (index sa stale posuva a ked dosiahne koniec, pride na zaciatok)
	Count++; //zvysi sa celkovy pocet prvkov vo fronte
}

//vrati prvy prvok
unsigned int List::get(void){
	unsigned int tmp = 0; //predbezne je to 0
	if (Count > 0) { //ak vo fronte este nieco mame
		Count--; //znizime celkovy pocet
		tmp = f[Start]; //prvy pripravime na vratenie
		f[Start] = 0; //zmazeme ho z fronty
		Start = (Start + 1) % Size; //posunieme zaciatok (znovu 'zacykleny' posun)
	}
	return (tmp);
}

//vracia pocet prvkov v liste
unsigned int List::getSize(void){
	return(Count);
}

//eliminuje duplikaty
void List::dupkick(void){
	unsigned int * tmp = new unsigned int[Count];
	int rpc;
	int size=Count;
	for (rpc=0; rpc<size;rpc++){
		tmp[rpc]=get();
	} 
	assert(get()==0);assert(getSize()==0);
    qsort(tmp,size, sizeof(unsigned int),compare);
    for(rpc=1; rpc<size; rpc++) 
		if(tmp[rpc]==tmp[rpc-1]) tmp[rpc-1]=0;//vynuluje duplicity
	for(rpc=0; rpc<size; rpc++)
		if(tmp[rpc]!=0) put(tmp[rpc]);
}