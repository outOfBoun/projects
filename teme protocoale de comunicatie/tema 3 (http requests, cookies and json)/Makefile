#S=-g
build: client

client:	client.o requests.o helpers.o parson.o
	g++ $S -Wall $^ -o $@

client.o: client.cpp 
	g++ $S -Wall -c $^ -o $@

requests.o: requests.cpp
	g++ $S -Wall -c $^ -o $@

helpers.o: helpers.cpp
	g++ $S -Wall -c $^ -o $@

parson.o : parson.cpp
	g++ $S -Wall -c $^ -o $@

clean:
	rm -rf client.o requests.o helpers.o parson.o client
