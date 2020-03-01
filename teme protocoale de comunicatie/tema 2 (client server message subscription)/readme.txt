Voicu Alex-Georgian

		Tema 2  -  Protocoale de Comunicatie  -  Aplicatie client-server TCP si UDP pentru gestionarea mesajelor


		msgutils.cpp:

	- tcprecv si tcpsend -> functii care citesc si scriu un numar specificat de octeti din stream-ul TCP


		client subscriber.cpp:

	- trimiterea de comenzi parsate prin structura Subscription.
	- receptionarea de mesaje semi-parsate de la server pe un anumit topic si interpretarea campului content(payload) din struct fmsg pentru afisarea corecta a tipurilor posibile de date.
	- mecanismul de exit este unul 'soft', bazat pe shutdown care permite transmiterea mesajelor aflate in retea pana la confirmarea inchiderii de catre server.


		server server.cpp:

	- comanda exit inchide serverul nu inainte de a dealoca memoria alocata pe heap si de a inchide socketii clientilor.
	- mesajele primite de la UDP se transmit instant clientilor TCP conectati si daca este cazul se stocheaza pentru clientii TCP care au SF = 1 pentru mesajul respectiv.
	- conectarea sau reconectarea unui client TCP poate duce la transmiterea mesajelor pe care acestia le-au pierdut si aveau SF = 1 pentru topicul respectiv.
	- comenzile clientilor TCP sunt interpretate si efectuate.

		Din punct de vedere al datelor, server-ul tine cont de:

		-> ClientDB - baza de date pentru clienti si preferintele lor, topicuri, SF, socket pe care poate functii accesat.
					- unordered_map pentru acces rapid al clientilor pe baza de ID sau SOCKET.
					- set pentru multimea de topicuri cu SF si fara SF.

		-> Msg - structura unui mesaj, cu dependite catre clientii deconectati care au SF.
			   - durata de viata dinamica, sters cand a fost trimis tuturor clientilor cu SF.
			   - mesajele unui topic ajung ordonate (liste dinamice ordonate).

		-> unordered_map<string, list<Msg *>> topicsToMsg - mapare de la topicuri la lista de mesaje a acelui topic
		
		-> unordered_map<string, set<ClientsDB::Client *>> topicsToCl; - mapare de la topicuri la clienti
