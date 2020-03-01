// Voicu Alex-Georgian

		Tema 1 PCom - Protocol cu fereastra glisanta
	


	Pachetele transmise sunt incapsulate in structura packet care umple campul payload al structurii msg.
	Functiile care prelucreaza pachetele si le transforma sunt:
	-pt send:
		- buffer_to_packet : transforma o zona de memorie intr-un packet, calculand checksum;
		- packet_to_msg : transforma un packet in msg;
		- msg_to_ack : transforma un mesaj primit intr-o structura de tip acknowledge;

	-pt recv:
		- packet_to_buffer : copiaza din packet payload-ul intr-o zona de memorie specificata, calculand mai intai checksum-ul pe care il compara cu checksum-ul primit pentru a vedea daca exista corupere;
		- msg_to_packet : transforma un msg in packet;
		- ack_to_msg : transforma un ack intr-un mesaj pentru a fi transmis inapoi sender-ului;

	In etapa initiala se transmit pachete de tip handshake pentru a specifica receiver-ului numele si dimensiunea fisierului ce trebuie transmis, cat si dimensiunea ferestrei si numarul total de pachete.

	Transmiterea pachetelor ce reprezinta fisierul se face folosind un protocol selectiv de retransmitere a pachetelor esuate.
	Atat sender-ul cat si receiver-ul tin cont de pachetele transmise cu succes (primit ack) si primite cu succces (a sosit necorupt)

	Sender-ul trimite, de fiecare data cand a iesit din recv_message_timeout cu timeout, o rafala de mesaje de dimensiunea ferestrei pentru a umple legatura de date. Cand primeste un ACK marcheaza ca trimis pachetul respectiv si trimite urmatorul pachet indicat de nt. Cand primeste NACK retransmite pachetul urmator.

	Recever-ul primeste mesaje in bucla si le marcheaza pe cele primite cu succes. Trimite dupa caz ACK sau NACK.
