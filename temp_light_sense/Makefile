IDIR=include/
SDIR=source/
CC=gcc
CFLAGS=-I$(IDIR)
RFLAGS=-rf

execute: main.o i2c_wrapper.o
	$(CC) -o execute main.o i2c_wrapper.o $(CFLAGS)

i2c_wrapper.o: $(SDIR)i2c_wrapper.c $(IDIR)i2c_wrapper.h
	$(CC) -c $(SDIR)i2c_wrapper.c

main.o: main.c $(IDIR)i2c_wrapper.h
	$(CC) -c main.c
.PHONY: clean

clean:
	rm $(RFLAGS) *.o $(SDIR)*.o execute
