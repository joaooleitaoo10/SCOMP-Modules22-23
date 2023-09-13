ex05: main.o
	gcc -g -Wall main.o -o ex05
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o	
	
vg: ex05	
	valgrind ./ex05
	
c:
	rm -f *.o ex05
	
r: ex05
	./ex05