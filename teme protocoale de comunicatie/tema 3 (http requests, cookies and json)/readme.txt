Voicu Alex-Georgian

		Tema 3 - Protocoale de Comunicatie


Client.cpp:

	Cookie-urile sunt retinute intr-un map si inserate parse_cookies() cu fiecare response care seteaza cookie-uri.
	Cookie string-ul pus in header-ul cererilor este creat in create_cookie_str().
	In cazul cererilor de POST cu form_data sau pentru crearea de query parameters se foloseste functia create_form_data().

	Pentru fiecare etapa programul se comporta relativ similar:
		-> se deschide o conexiune TCP.
		-> se creaza mesajul de request cu informatiile initiale / citite in etapa precedenta.
		-> se asteapta raspunsul si se salveaza cookie-urile.
		-> se parseaza fisierul json primit daca este cazul si se identifica setarile obiectului.
		-> se modifica parametrii de apel si informatiile pentru etapa urmatoare.
		-> se elibereaza memoria alocata in lucrul etapei respective si se inchide conexiunea.

	
