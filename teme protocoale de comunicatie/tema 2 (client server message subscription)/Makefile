build: server subscriber

server:	server.o msgutils.o
	g++ -Wall $^ -o $@

subscriber: subscriber.o msgutils.o
	g++ -Wall $^ -o $@

server.o: server.cpp
	g++ -Wall -c $^ -o $@

subscriber.o: subscriber.cpp
	g++ -Wall -c $^ -o $@

msgutils.o: msgutils.cpp
	g++ -Wall -c $^ -o $@

clean:
	rm -rf server.o subscriber.o msgutils.o server subscriber