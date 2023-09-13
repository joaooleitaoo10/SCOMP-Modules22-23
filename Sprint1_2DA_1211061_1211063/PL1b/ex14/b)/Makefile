ex14: main.o
	gcc -g -Wall main.o -o ex14
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o		
	
vg: ex14
	valgrind ./ex14
	
c:
	rm -f *.o ex14
	
r: ex14
	./ex14