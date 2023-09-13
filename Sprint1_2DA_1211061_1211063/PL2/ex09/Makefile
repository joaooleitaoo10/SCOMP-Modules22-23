ex09: main.o
	gcc -g -Wall main.o -o ex09
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o	
	
vg: ex09
	valgrind ./ex09
	
c:
	rm -f *.o ex09
	
r: ex09
	./ex09