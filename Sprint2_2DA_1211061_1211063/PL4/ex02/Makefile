reader : reader.c
	gcc -g -Wall -lrt reader.c -o reader

writer : writer.c
	gcc -g -Wall -lrt writer.c -o writer
	
c:
	rm -f *.o reader writer

rw: writer
	./writer

rr: reader
	./reader