CC=gcc
CFLAGS=-Wall -Wextra -c

build: capt.o
	$(CC) $^ -o captexe

capt.o: main.c
	$(CC) $(CFLAGS) $^ -o $@

run:
	./captexe

clean:
	rm -rf capt.o captexe


