CC=gcc
CFLAGS += -Wall -Wextra -Wwrite-strings -Wno-parentheses -Wpedantic -Warray-bounds
CFLAGS += -g
LDFLAGS=
LIBS=

shell: parse.o exec.o shell.o
	    $(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	    $(CC) $(CFLAGS) -c -o $@ $^

clean:
	    rm *.o shell
