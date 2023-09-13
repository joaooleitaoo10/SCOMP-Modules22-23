ex15: main.o
	gcc -g -Wall main.o -o ex15
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o	
	
vg: ex15	
	valgrind ./ex15
	
c:
	rm -f *.o ex15
	
r: ex15
	./ex15