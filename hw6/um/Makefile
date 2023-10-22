#
# Makefile for the UM Test lab
# 
CC = gcc

IFLAGS  = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64
LDLIBS  = -lbitpack -larith40 -l40locality -lcii40 -lm 

EXEC   = um um_test

all: $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

um: main.o registers.o segment.o 
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

um_test: umtest.o umwrite.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)







# main: main.o registers.o segment.o 
# 	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# registers: main.o registers.o segment.o 
# 	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# segment: main.o registers.o segment.o 
# 	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)




clean:
	rm -f $(EXEC)  *.o
