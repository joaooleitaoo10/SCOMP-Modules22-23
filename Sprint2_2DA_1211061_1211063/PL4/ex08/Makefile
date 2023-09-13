ex08: main.o insert.o consult.o consult_all.o up_down.o
	gcc -g -Wall -lrt main.o insert.o consult.o consult_all.o up_down.o -o ex08
	
main.o: main.c
	gcc -g -Wall -c main.c -o main.o

insert.o: insert.c
	gcc -g -Wall -c insert.c -o insert.o

consult.o: consult.c
	gcc -g -Wall -c consult.c -o consult.o

consult_all.o: consult_all.c
	gcc -g -Wall -c consult_all.c -o consult_all.o	

up_down.o: up_down.c
	gcc -g -Wall -c up_down.c -o up_down.o		
	
vg: ex08
	valgrind ./ex08
	
c:
	rm -f *.o ex08
	
r: ex08
	./ex08