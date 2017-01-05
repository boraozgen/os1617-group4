# IPC Makefile

CC = gcc
CFLAGS  = -Wall -g
OBJ_DAEMON = ipcd.o
OBJ_CLIENT = ipc-client.o
LIBS = -lpthread -lrt

all: ipcd ipc-client

ipcd: $(OBJ_DAEMON)
	$(CC) $(CFLAGS) -o ipcd $(OBJ_DAEMON) $(LIBS)

ipc-client: $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o ipc-client $(OBJ_CLIENT) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
