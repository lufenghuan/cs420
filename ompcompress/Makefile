OBJS = compress.o serialcompress.o
CC = gcc
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

serialcompress : $(OBJS)
	$(CC) $(LFLAGS) -lz $(OBJS) -o serialcompress

compress.o : compress.h compress.c
	$(CC) $(CFLAGS) compress.c

serialcompress.o : compress.h serialcompress.c
	$(CC) $(CFLAGS) serialcompress.c

clean:
	rm *.o *serialcompress

