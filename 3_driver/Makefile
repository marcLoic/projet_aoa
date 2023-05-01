CC=clang
CFLAGS=-O2  -Wall
OPTFLAGS=-O2  -Wall
OBJS_COMMON=kernel.o

all: driver_check

driver_check.o: driver_check.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.o: kernel.c
	$(CC) $(OPTFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_COMMON) driver_check.o kernel.o
