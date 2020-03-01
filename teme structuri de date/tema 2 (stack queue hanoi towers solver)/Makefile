build: main.o functiiCoada.o functiiStiva.o functiiLG.o
	gcc -Wall $^ -o tema2

main.o: main.c
	gcc -Wall -c $^ -o $@

functiiCoada.o: functiiCoada.c
	gcc -Wall -c $^ -o $@

functiiStiva.o: functiiStiva.c
	gcc -Wall -c $^ -o $@

functiiLG.o: functiiLG.c
	gcc -Wall -c $^ -o $@

clean:
	rm -rf main.o functiiStiva.o functiiCoada.o functiiLG.o tema2
