# IPC Makefile

CC = gcc
CFLAGS  = -Wall -g
OBJ_DAEMON = ipcd.o
OBJ_CLIENT = ipc-client.o

all: ipcd ipc-client

ipcd: $(OBJ_DAEMON)
	$(CC) $(CFLAGS) -o ipcd $(OBJ_DAEMON)

ipc-client: $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o ipc-client $(OBJ_CLIENT)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
