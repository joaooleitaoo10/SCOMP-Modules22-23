ex01: main.o
	gcc -g -Wall main.o -o ex01
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o	
	
vg: ex01	
	valgrind ./ex01
	
c:
	rm -f *.o ex01
	
r: ex01
	./ex01