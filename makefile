hours: hours.o
	$(CC) -o hours hours.o

hours.o: hours.c
	$(CC) -c hours.c

clean:
	rm hours hours.o
