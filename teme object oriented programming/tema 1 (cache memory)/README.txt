VOICU Alex-Georgian

##### Tema 1 - Give me more subscriptions

	In Main are loc citirea datelor, crearea cacheului potrivit si pasarea acestuia catre obiectul Memorie. In continuare urmeaza bucla pentru citirea si interpretarea comenzilor.
	In Memorie:
		- metoda addToMem() parcurge lista de subscriptii. Daca obiectul deja exista atunci este sters atat din aceasta cat si din cache(daca exista si acolo, lucru verificat de cache). In final obiectul de introdus este adaugat listei memoriei.
		- metoda get() construieste un obiect Free avand numele obiectului ce trebuie cautat. Deoarece metoda equals este suprascrisa pentru clasa Subscriptie, se vor compara doar numele acestora, nu si cererile ramase. Obiectul este astfel cautat mai intai in cache. Daca nu este gasit este cautat apoi in memorie.
		
	FIFOcache:
		- implementarea este facuta printr-o coada.
		
	LRUCache:
		- pentru a nu folosi un timestamp, se utilizeaza o lista in care inseram mereu la sfarsit elementul tocmai inserat sau accesat. Astfel cele mai recente elemente vor fi aflate catre sfarsitul cozii.
		- metoda search(), deci, dupa ce gaseste elementul ce trebuie cautat il plaseaza la sfarsit, daca exista.
		
	Clasele Subscriptie, Free, Basic si Premium:
		- utilizeaza mostenirea prin lantul constructorilor in cascada cat si prin metodele getType() si decrease(), care utilizeaza local datele clasei respective si apoi paseaza decizia mai departe prin super().
		
		
##### Bonus
	
	LFUCache:
		- obiectul Pair impacheteaza obiectele ce vor fi adaugate in cache impreuna cu un contror de utilizare.
		- obiectele sunt inserate in cache doar la finalul listei, astfel incat vechimea obiectelor creste spre finalul listei.
		- la stergere este cautat obiectul cu valoarea frecventei de utilizare cea mai mica si aflat cel mai spre inceputul listei.
