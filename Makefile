CFLAGS = -D_FORTIFY_SOURCE=2 -O2 -g -Wall -Wextra -Werror -Wno-unused-parameter
LDFLAGS = -pthread

all: test

test: test.o 

.PHONY: clean

clean:
	rm *.o test
