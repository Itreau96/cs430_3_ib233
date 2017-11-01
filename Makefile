# Simple makefile

CC = gcc
CFLAGS = -g -Wall

all: raycast

# Create raycaster
raycast: raycast.c raycast.h ib_3dmath.h parser.c parser.h
	$(CC) $(CFLAGS) raycast.c raycast.h ib_3dmath.h parser.c parser.h -o raycast -lm

# Create clean
clean:
	-rm -rf raycast *~
