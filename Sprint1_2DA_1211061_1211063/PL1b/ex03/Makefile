ex03: main.o
	gcc -g -Wall main.o -o ex03
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o	
	
vg: ex03	
	valgrind ./ex03
	
c:
	rm -f *.o ex03
	
r: ex03
	./ex03