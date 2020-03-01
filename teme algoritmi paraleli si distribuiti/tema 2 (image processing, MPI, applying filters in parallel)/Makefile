DEB=#-g
build: tema3

tema3: tema3.o
	mpic++ $^ $(DEB) -Wall -o $@

tema3.o: tema3.cpp
	mpic++ -c $^ $(DEB) -Wall -o $@

clean:
	rm -rf tema3.o tema3