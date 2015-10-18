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
kazdy test bude napisany v hlavnom zdrojovom subore, komentar bude obsahovat 
popis testu a nazov vystupneho suboru. Vystupne subory by mali obsahovat skript
SQL ktory vklada udaje do databazy(pre dalsie spracovanie).
(Vytvorenie databazy bude zvlast.)

Pre kazdy beh algoritmu hladania cesty plati, ze ak cesta medzi zadanymi 
vrcholmi neexistuje, pokus sa nepocita. (pretoze predpokladame vstupnu ulohu
ktora ma riesenie v podobe najdenej cesty)

IMPLEMENTACIE

kazda implementacia ma nazov v nejakej standardnej frome, nech je to XXX, 
uloha obsahuje Verts - vrcholy, Edges - hrany, Src,Dst identifikator
pociatocneho a koncoveho vrcholu a JobID (jedinecny) identifikator ulohy.
Bude metoda vracajuca cestu XXXPath ako retazec identifikatorov 
vrcholov oddelenych ciarkami(po skonceni riesenia).
XXXSolve bude sluzit na spustenie riesenia. XXXFinish uvedie kniznicu do 
povodneho stavu ako na zaciatku, tj uvolni retazec riesenia aj strukturu so 
zadanim.

Stopuje sa cas behu XXXSolve.

Potom sa preberu vysledky XXXPath a zavola sa XXXFinish.


/////////////////////////////////////////////////////////////////////////////
