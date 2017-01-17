# MAXSUM Makefile

CC = gcc
CFLAGS  = -Wall -g
OBJ = maxsum.o

all: maxsum

gbsd: $(OBJ)
	$(CC) $(CFLAGS) -o maxsum $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
