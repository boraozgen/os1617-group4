# GBS Daemon Makefile

CC = gcc
CFLAGS  = -Wall -g
OBJ = gbsd.o

all: gbsd

gbsd: $(OBJ)
	$(CC) $(CFLAGS) -o gbsd $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
