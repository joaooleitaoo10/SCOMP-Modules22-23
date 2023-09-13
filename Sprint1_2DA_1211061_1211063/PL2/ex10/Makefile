ex10: main.o
	gcc -g -Wall main.o -o ex10
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o		
	
vg: ex10
	valgrind ./ex10
	
c:
	rm -f *.o ex10
	
r: ex10
	./ex10
