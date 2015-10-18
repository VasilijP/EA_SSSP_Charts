#pragma once

/*
Trieda urcena na uchovavanie zoznamu prirodzenych cisel a manipulaciu s nimi
(tj hodnoty 1,2,3..., !!!nie 0!!!)
*/
class List {
private:
	unsigned int * f;
	unsigned int Size;
	unsigned int Count;
	unsigned int Start;
	unsigned int End;
public:
	bool empty();
	bool contains(unsigned int o);
	void put(unsigned int v);
	unsigned int get();
	unsigned int getSize();
	void dupkick();
	List(void);
	~List(void);
};

	//f = NULL;//uloziste prvkov
	//Size = 0;//alokovana velkost
	//Count = 0;//pocet poloziek
	//Start = 0;//index prveho
	//End = 0;//index prveho volneho miesta