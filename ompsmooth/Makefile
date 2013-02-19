CC=gcc
CFLAGS = -fopenmp

all: smooth

smooth: smooth.o ompsmooth.o
	$(CC) $(CFLAGS) -o smooth smooth.o ompsmooth.o

clean:

	rm *.o
	rm smooth


