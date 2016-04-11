GCC = "gcc"
CFLAGS = -g -Wall

default: build

build: main.c
	$(GCC) $(CFLAGS) -o build main.c

clean:
	$(RM) build *.o *~