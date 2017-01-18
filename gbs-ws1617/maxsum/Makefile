# MAXSUM Makefile

CC = gcc
CFLAGS  = -Wall -g
OBJ = maxsum.o
LIBS = -lpthread

all: maxsum

maxsum: $(OBJ)
	$(CC) $(CFLAGS) -o maxsum $(OBJ) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
