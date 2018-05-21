CFLAGS = -g3
LDFLAGS = -pthread

all: test

test: test.o 

.PHONY: clean

clean:
	rm *.o test
