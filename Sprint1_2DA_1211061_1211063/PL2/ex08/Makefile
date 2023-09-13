ex08: main.o
	gcc -g -Wall main.o -o ex08
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o		
	
vg: ex08
	valgrind ./ex08
	
c:
	rm -f *.o ex08
	
r: ex08
	./ex08