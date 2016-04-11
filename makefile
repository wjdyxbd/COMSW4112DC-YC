default: build

build: main.c
	gcc -g -Wall -o build main.c

clean:
	$(RM) build *.exe *.out *.o *~