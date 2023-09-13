ex12: main.o
	gcc -g -Wall main.o -o ex12
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o		
	
vg: ex12
	valgrind ./ex12
	
c:
	rm -f *.o ex12
	
r: ex12
	./ex12
