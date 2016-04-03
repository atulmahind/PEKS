CC=gcc
SOURCE=main.c peks.c
DEPS=peks.h
EXECUTABLE=peks
CFLAG=-I ~/.local/include/pbc
LDFLAGS=-Wl,-rpath ~/.local/lib -lpbc -lssl -lcrypto -lgmp -lm


target:
	$(CC) $(SOURCE) -o $(EXECUTABLE) $(CFLAG) $(LDFLAGS)
