ex12: main.o up_down.o
	gcc -g -Wall -lrt main.o up_down.o -o ex12
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o


up_down.o: up_down.c
	gcc -g -Wall -c up_down.c -o up_down.o	
	
vg: ex12
	valgrind ./ex12
	
c:
	rm -f *.o ex12
	
r: ex12
	./ex12