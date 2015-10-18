========================================================================
    CONSOLE APPLICATION : Zhrnutie
========================================================================

Tento program je sucastou prace na predmet Experimentalna analyza algoritmov.
Obsahuje nastroj na generovanie vstupnych dat, implementacie porovnavanych algoritmov a
testovaci program, ktory vykona potrebne testy a zapise namerane hodnoty do suborov.

PathFinding.cpp
    Hlavny spustitelny subor, spusta testy podla stanoveneho scenara na 
    vygenerovanych datach.

/////////////////////////////////////////////////////////////////////////////
Ostatne poznamky:

GENERATOR
implementovat generator kde bude volitelny pocet vrcholov(1) grafu, 
prepojenost(2), charakter rozmiestnenia vrcholov(3), dlzky ciest(4).
Vsetky hodnoty budu celociselne v max rozmedzi 32bit.
Algoritmus musi fungovat aj pre nesuvisle grafy, takze suvislost
nebude v generatore overovana.
(1) - pocet vrcholov charakterizovany jednym cislom pre SPACE aj FLAT
(2) - pocet hran v % z poctu vsetkych moznych 
		alebo 
      min poctu susedov pre kazdy vrchol
    - priznak NEAREST ci bude vrchol prepajany s najblizsimi susedmi, 
      ma sluzit na simulovanie cestnej siete a podobnych struktur      
(3) - SPACE = nahodne rozmiestnenie v guli so stredom [0,0,0]      
      FLAT = nahodne rozmiestnenie v kruhu [0,0] (tj Z suradnica = 0)      
(4) - v % o kolko moze byt dlzka hrany vacsia od priamej vzdialenosti,
      u SPACE aj FLAT ide o usecku medzi vrcholmi, 0% = eukl. metrika R^3, 
      inak ide o povodnu vzdialenost + nahodne priratanie 0 az zadany pocet %
Generator musi umoznovat viacnasobne citanie vygenerovaneho grafu.
Na citanie bude sluzit metoda getJob(), vrati strukturu s ulohou.
V pripade ze nemozno graf z danymi parametrami vygenerovat, je RV NULL.

TESTY
Test bude tvoreny testovacou davkou, hlavny program dostane nazov vstupnej davky
ako parameter, presny format bude popisany neskor, ale vystupom kazdej davky bude sql skript
vkladajuci vysledky do databazy.

IMPLEMENTACIE

uloha obsahuje: 
 Verts - vrcholy, (aj pocet vrcholov)
 Edges - hrany, (aj pocet hran) 
 Src,Dst identifikator pociatocneho a koncoveho vrcholu a 
 JobID (jedinecny) identifikator ulohy, 

pomocou JobID je mozne ulohu znovu vytvorit (ak budu zname aj ostatne parametre ulohy)
Riesenie bude spocivat v jedinej metode, ktora dostane jediny parameter - ukazatel na strukturu s ulohou a vystupom bude ukazatel na riesenie. 
Riesenim ulohy je retazec cisel oddelenych ciarkou, na zaciatku je v zatvorke dlzka cesty, potom nasleduje zoznam cisel, prve cislo je cislo pociatocneho vrcholu a posledne cislo je cislo koncoveho vrcholu, ostatne cisla budu po poradi cisla vrcholov ako nasleduju na lubovolnej z najkratsich ciest z pociatocneho do koncoveho vrchola.
Ak riesenie pre danu ulohu nieje, retazec obsahuje iba 'NONE'

Stopuje sa cas behu riesenia pomocou instrukcie RDTSC.

FORMAT Testovacieho scenara

prvy riadok - nazov vystupneho skriptu, bez pripony sql
 dalej lubovolne krat
1. riadok - nazov testovaneho algoritmu
2. riadok - prepinace oddelene medzerami (SPACE, FLAT...)
3. riadok - cele cisla po poradi: vertNo, eLenAdd, conn, taskId (pre -1 sa pouzije nahodny taskId)
posledny riadok - 'END'
/////////////////////////////////////////////////////////////////////////////
