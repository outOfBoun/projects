build: main.o functiiLG.o
	gcc -Wall $^ -o tema3 -lm

main.o: main.c
	gcc -Wall -c $^ -o $@

functiiLG.o: functiiLG.c
	gcc -Wall -c $^ -o $@

clean:
	rm -rf main.o functiiLG.o tema3
