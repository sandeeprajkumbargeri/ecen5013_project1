IDIR=include/
SDIR=source/
CC=gcc
CFLAGS=-g -lm -lpthread -lrt
RFLAGS=-rf

execute: main.o i2c_wrapper.o apds_9301_driver.o tmp_102_driver.o
	$(CC) -o execute main.o i2c_wrapper.o apds_9301_driver.o $(CFLAGS)

i2c_wrapper.o: $(SDIR)i2c_wrapper.c $(IDIR)i2c_wrapper.h
	$(CC) -c -g $(SDIR)i2c_wrapper.c

apds_9301_driver.o: $(SDIR)apds_9301_driver.c $(IDIR)apds_9301_driver.h $(IDIR)i2c_wrapper.h
	$(CC) -c -g $(SDIR)apds_9301_driver.c

tmp_102_driver.o: $(SDIR)tmp_102_driver.c $(IDIR)tmp_102_driver.h $(IDIR)i2c_wrapper.h
	$(CC) -c -g $(SDIR)tmp_102_driver.c

main.o: main.c $(IDIR)i2c_wrapper.h $(IDIR)apds_9301_driver.h
	$(CC) -c -g main.c
.PHONY: clean

clean:
	rm $(RFLAGS) *.o $(SDIR)*.o execute
