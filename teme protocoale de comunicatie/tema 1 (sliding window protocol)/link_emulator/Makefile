all: link lib.o

link: link.o queue.o
	gcc -g -pthread link.o queue.o -o link -O3

.c.o: 
	gcc -Wall -g -pthread -c $? -O3

clean:
	rm -f *.o link
