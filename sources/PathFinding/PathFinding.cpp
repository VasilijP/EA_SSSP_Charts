// PathFinding.cpp : Defines the entry point for the console application.
//

#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include "gen.h"
#include "types.h"
#include "solutions.h"

using namespace std;

typedef unsigned __int64 t_duration_cycles;

t_duration_cycles __stdcall get_cycles (void);
unsigned int getSwitch(string s);
string solutionToStr(const SOLUTION * s);

int _tmain(int argc, _TCHAR* argv[])
{	
	/*INIT*/
	if (argc < 2){
		printf("%s","Usage is: PathFinding InputScript");
		return 0;
	}

	ifstream IN(argv[1]);//vstupny subor
	string BatchName;
	string OFName;//nazov vysupneho suboru
	getline(IN,BatchName,'\n');//nacita prvy riadok s nazvom vystupneho suboru
	OFName = "output\\"+BatchName+".sql";//prida cestu a priponu
	ofstream OUT(OFName.data(),ios_base::out);//otvori vystup
	/*INIT end*/

	string AName;//nazov testovaneho algoritmu
	string Switches;//riadok prepinacov
	int vertNo, eLenAdd, conn, TaskId;//parametre grafu
	int flags; //flagy pre generovanie


	IN >> AName;
	while (AName != "END"){//kym niesme na konci skriptu, nacitame parametre a spustime spracovanie
		IN >> noskipws;	
		flags = 0;
		do {		
			IN.ignore();
			IN >> Switches;//nacitanie prepinacov
			flags |= getSwitch(Switches);
			if (getSwitch(Switches)==0) OUT << "-- Unknown switch found: " << Switches << endl;
		} while (IN.peek()!='\n');
		IN.ignore();
		IN >> vertNo;
		IN.ignore();
		IN >> eLenAdd;
		IN.ignore();
		IN >> conn;
		IN.ignore();
		IN >> TaskId;//nacitanie parametrov pre generovanie		

		if (TaskId == -1){//vytvori ulohu s nahodnym TaskId
			genJob(flags,vertNo,eLenAdd,conn);
		} else {//vytvori ulohu so zadanym TaskId
			genJob(flags,vertNo,eLenAdd,conn,TaskId);
		}

		/*ak nie je graf vygenerovany, zapisat chybu, inak...*/
		if (getJob()==NULL){
			OUT << "-- Unable to create JOB" << endl;
			OUT << "INSERT INTO result(alg,vertcount,elenadd,conn,batchname,result) \nVALUES('";
			OUT << AName << "',\n" << vertNo << ",\n" << eLenAdd << ",\n" << conn <<",\n'" << BatchName <<"',\n'ERROR');" << endl << endl;
		} else {/*NAJDENIE zodpovedajuceho algoritmu a spustenie riesenia a zapisanie vysledkov*/
			int rpc = 0;
			while ((rpc < ALGCOUNT)&&(AlgNames[rpc] != AName)) rpc++;
			if (rpc >= ALGCOUNT){//nenasiel sa zodpovedajuci algoritmus
				OUT << "-- Unable to find algorithm " << AName << endl;
				OUT << "INSERT INTO result(alg,vertcount,elenadd,conn,batchname,result) \nVALUES('";
				OUT << AName << "',\n" << vertNo << ",\n" << eLenAdd << ",\n" << conn <<",\n'" << BatchName <<"',\n'ERROR');" << endl << endl;
			} else {//mame algoritmus Algs[rpc] a treba ho spustit na getJob()
				t_duration_cycles alg_started, alg_stopped, alg_duration;
				SOLUTION * result;
				JOB * j = getJob();
				int w = 9 + AName.length();

				//odstopovat cas
				alg_started = get_cycles();
				//spustit algoritmus
				result = Algs[rpc](j);
				//odstopovat cas
				alg_stopped = get_cycles();
				alg_duration = (unsigned __int64)(alg_stopped - alg_started);
				string resultStr = solutionToStr(result);				
				delete[] result->V;
				delete result;
				//zapisat udaje, vysledok a dlzku behu
				OUT << "-- Test finished" << endl;
				OUT << "INSERT INTO result(alg,vertcount,edgecount,elenadd,conn,task,cycles,batchname,result) \nVALUES('";
				OUT << AName << "',\n" << setw(w) << vertNo << ",\n" << setw(w) << j->edgeNo;
				OUT << ",\n" << setw(w) << eLenAdd << ",\n" << setw(w) << conn;
				OUT << ",\n" << setw(w) << j->taskId << ",\n" << setw(w) << alg_duration;
				OUT << ",\n" << setw(w-1) << "'" + BatchName << "',\n" << setw(w-1) << "'" + resultStr << "');" << endl << endl;				
			}
		}

		IN.ignore();
		IN >> AName;//nacita dalsi riadok s nazvom algoritmu alebo 'END'
	}	

	return 0;
}

unsigned int getSwitch(string s){
	if (s == "SPACE") return(SPACE);
	if (s == "FLAT") return(FLAT);
	if (s == "NEAREST") return(NEAREST);
	if (s == "PEREDGECONN") return(PEREDGECONN);
	if (s == "MINEDGECONN") return(MINEDGECONN);
	return(0);
}

t_duration_cycles __stdcall get_cycles (void)
{
	unsigned __int64 result = 0;
	unsigned __int32 hi = 0, lo = 0;

	__asm
	{
		rdtsc
			mov hi, eax
			mov lo, edx
	};

	result = (unsigned __int64)hi + (((unsigned __int64)lo) << 32);

	return result;
};

string solutionToStr(const SOLUTION * s){
	if (!s->exists) return(string("NONE"));	
		
	stringstream rvs;	
	rvs << '(' << s->l << ')' << s->V[0];	
	for (int rpc = 1; rpc<s->vertNo; rpc++){
		rvs << ',' << s->V[rpc];
	}
	return(rvs.str());
}
