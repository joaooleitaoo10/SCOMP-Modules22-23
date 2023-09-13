ex04: main.o
	gcc -g -Wall main.o -o ex04
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o		
	
vg: ex04
	valgrind ./ex04
	
c:
	rm -f *.o ex04
	
r: ex04
	./ex04