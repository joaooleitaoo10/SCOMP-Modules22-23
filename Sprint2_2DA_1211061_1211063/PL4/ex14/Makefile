ex14: main.o up_down.o
	gcc -g -Wall -lrt main.o up_down.o -o ex14
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o


up_down.o: up_down.c
	gcc -g -Wall -c up_down.c -o up_down.o	
	
vg: ex14
	valgrind ./ex14
	
c:
	rm -f *.o ex14
	
r: ex14
	./ex14