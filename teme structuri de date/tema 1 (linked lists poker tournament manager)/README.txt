//VOICU Alex-Georgian

	Tema 1 - Poker Tournament
	
		Crearea salii.
	Sala, lista de mese si lista de jucatori sunt create si initializate prin intermediul functiilor creareSala, creareMasa si creareJucator. 
	Prima functie deschide fisierul de configuratie, aloca si initializeaza datele salii, apoi porneste o bucla in care vor fi create (prin creareMasa) si inlantuite mesele din sala.
	La fiecare accesare a functiei creareMasa are loc un proces asemanator celui anterior. Adica se initializeaza valorile mesei respective si apoi porneste o bucla ce creeaza jucatorii (prin creareJucator) si ii inlantuie in listele circulare din care fac parte.
	Functia creazaJucator este functia cea mai adanca, in care se aloca spatiul necesar si se citesc datele jucatorului.
	
		Eliminarea/ dealocarea structurilor si listelor
	Eliberarea informatiei alocate dinamic se realizeaza prin functiile eliminareSala, eliminareMasa si eliminareJucator. Prin chemarea uneia dintre functiile anterioare are loc si eliberarea substructurilor lor. (eliminareSala elimina tot; eliminareMasa elimina atat informatia mesei cat si jucatorii din lista).
	Pentru a sterge informatia cat si listele(circulare sau necirculare) se foloseste functia Distruge, careia ii sunt date ca parametru una din functiile de eliminare.
	Procedeul de distrugere functioneaza oarecum recursiv, de exemplu:
-eliminareSala apeleaza Distruge asupra listei de mese.
-in distrugerea listei de mese se va apela eliminareMasa pe fiecare celula a listei de mese.
-eliminareMasa apeleaza Distruge asupra listei de jucatori.
-in distrugerea listei de jucatori se va apela eliminareJucator pe fiecare celula a listei de jucatori.

		Functiile scoate_jucator si scoate_masa sunt folosite pentru a elimina un jucator ramas fara maini si, respectiv, pentru a inlatura o masa ramasa fara jucatori.
		
		Functia print
	Parcurge sala, lista de mese si lista de jucatori si afiseaza structura acesteia in mod corespunzator.
		
		Functia noroc
	Consta doar in parcurgerea listei de mese si apoi a listei de jucatori pentru a putea creste numarul de maini al jucatorului specificat in cazul in care acesta exista.
	
		Functia ghinion
	Cautarea jucatorului se face aidoma functiei noroc. Atunci cand se scade numarul de maini se verifica daca jucatorul mai ramane la masa. Daca nu, atunci jucatorul este scos. Se verifica apoi daca masa mai are jucatori. Daca nu, atunci masa este scoasa.
	
		Functia tura
	Se cauta masa si daca este gasita se face rotatia, mutand primul jucator in spatele santinelei. O bucla parcurge lista de jucatori a mesei si decrementeaza numarul de maini al fiecarui jucator. Se verifica daca jucatorul mai ramane la masa. Daca nu, atunci jucatorul este scos. Se verifica apoi daca masa mai are jucatori. Daca nu, atunci masa este scoasa.
	
		Functia tura completa
	O bucla ce parcurge lista de mese produce pasii unei ture la fiecare masa.
	
		Functia inchide
	Se fac verificarile necesare pentru a vedea daca este suficient loc in sala folosind variabilele din structura sala ce memoreaza numarul total de jucatori si numarul curent de jucatori.
	Daca este suficient loc pentru redistribuirea jucatorilor, se pastreaza un pointer la masa ce este scoasa din lista de mese, dar nu eliberata.
	Se parcurge lista meselor ramase. Se afla sfarsitul listei jucatorilor fiecarei mese, prin parcurgerea listei respective, si se adauga la masa respectiva jucatori din masa scoasa cat timp mai e loc sau mai sunt jucatori de redistribuit.
	La final se elimina masa redistribuita care va mai avea doar santinela, restul celulelor fiind mutate in celelalte liste de jucatori ale meselor la care au fost redistribuiti jucatorii.
	
		Functia clasament
	Se cauta masa ceruta si daca este gasita se parcurge lista de jucatori, se aloca memorie pentru elementele de tip jucator si numele lor, se copiaza datele si se creeaza cu aceste structuri de informatii o noua lista prin inserare ordonata.
	Functia apelata este InsOrdLG ce primeste ca parametru un pointer catre functia de comparare comp_clasament, ce sorteaza dupa ordinea ceruta.
	Se afiseaza apoi lista astfel creata, iar apoi se elibereaza.
		
