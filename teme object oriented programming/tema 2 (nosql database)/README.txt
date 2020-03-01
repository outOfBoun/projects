VOICU Alex-Georgian

	TEMA 2 - Lost in NoSQL
	
	Database.java implementeaza metodele de lucru intr-o baza de date. Clasa interna Node modeleaza nodurile pe care se face replicarea instantelor.
	Astfel, in baza de date se tine o lista de noduri, iar in noduri se tin instantele de tipul EInstance. Totodata, baza de date tine un obiect Map de entitati de tipul Entity (in care se tin numele si tipul atributelor din instante).
	Deci nodurile tin doar informatia utila, care e apoi interpretata la nivelul bazei de date.
	
	Entity.java reprezinta obiecte de tip entitate care contin liste de atribute de definitie (nume si tip, nu valori).
	EInstance.java reprezinta obiecte de tip instanta a entitatilor. Acestea tin o referinta la tipul de entitate de care apartin si o lista de obiecte Attribute care contin valori pentru atribute de cele 3 tipuri.
	Clasa Attribute si derivatele modeleaza tipurile de atribute float, int si string.
	
	Inserarea unei entitati se face parcurgand lista de noduri pentru rf-ul necesar si inserand in lista de entitati a fiecarui nod la inceputul acesteia cate o copie a instantei nou create.
	Lista de noduri este mereu sortata descrescator dupa fiecare inserare pentru a insera mereu in cele mai ocupate noduri.
	
	Fulldatabase: in inserare se face o verificare a numarului de inserari reusite si daca mai e novoie se creeaza noduri suplimentare in care sa se insereze copii ale instantei now create.
	
	Celelalte functii constau in simple parcurgeri si identificari pe baza atributelor pentru a realiza comenzile.
