IDIR=include/
SDIR=source/
CC=gcc
CFLAGS=-g -lm -lpthread -lrt
RFLAGS=-rf

execute: main.o i2c_wrapper.o apds_9301_driver.o tmp_102_driver.o temp_sense_task.o light_sense_task.o sock_comm_task.o logger_task.o startup_tests.o
	$(CC) -o execute main.o i2c_wrapper.o apds_9301_driver.o tmp_102_driver.o temp_sense_task.o light_sense_task.o sock_comm_task.o logger_task.o startup_tests.o $(CFLAGS)

i2c_wrapper.o: $(SDIR)i2c_wrapper.c $(IDIR)i2c_wrapper.h main.h $(IDIR)logger_task.h
	$(CC) -c -g $(SDIR)i2c_wrapper.c

apds_9301_driver.o: $(SDIR)apds_9301_driver.c $(IDIR)apds_9301_driver.h $(IDIR)i2c_wrapper.h main.h $(IDIR)logger_task.h
	$(CC) -c -g $(SDIR)apds_9301_driver.c

tmp_102_driver.o: $(SDIR)tmp_102_driver.c $(IDIR)tmp_102_driver.h $(IDIR)i2c_wrapper.h main.h $(IDIR)logger_task.h
	$(CC) -c -g $(SDIR)tmp_102_driver.c

light_sense_task.o: $(SDIR)light_sense_task.c $(IDIR)light_sense_task.h $(IDIR)i2c_wrapper.h $(IDIR)apds_9301_driver.h main.h $(IDIR)logger_task.h
	$(CC) -c -g $(SDIR)light_sense_task.c

temp_sense_task.o: $(SDIR)temp_sense_task.c $(IDIR)temp_sense_task.h $(IDIR)i2c_wrapper.h $(IDIR)tmp_102_driver.h main.h $(IDIR)logger_task.h
	$(CC) -c -g $(SDIR)temp_sense_task.c
sock_comm_task.o: $(SDIR)sock_comm_task.c $(IDIR)sock_comm_task.h main.h $(IDIR)logger_task.h
	$(CC) -c -g $(SDIR)sock_comm_task.c
logger_task.o: $(SDIR)logger_task.c $(IDIR)logger_task.h main.h $(IDIR)logger_task.h
	$(CC) -c -g $(SDIR)logger_task.c
startup_tests.o: startup_tests.c startup_tests.h $(IDIR)apds_9301_driver.h $(IDIR)tmp_102_driver.h
	$(CC) -c -g startup_tests.c


main.o: main.c $(IDIR)i2c_wrapper.h $(IDIR)apds_9301_driver.h $(IDIR)tmp_102_driver.h main.h $(IDIR)logger_task.h
	$(CC) -c -g main.c
.PHONY: clean

clean:
	rm $(RFLAGS) *.o $(SDIR)*.o execute
