CFLAGS = -Wall -Werror -std=c11 -o s-talk.out
OFLAGS = -Wall -Werror -std=c11 -g -c -Og

all: s-talk.out

s-talk.out: main.o list.o node_manager.o list_manager.o
	gcc $(CFLAGS) main.o list.o node_manager.o list_manager.o

main.o: main.c
	gcc $(OFLAGS) main.c

list.o: list.c list.h node_manager.h list_manager.h
	gcc $(OFLAGS) list.c

node_manager.o: node_manager.c node_manager.h list.h
	gcc $(OFLAGS) node_manager.c

list_manager.o: list_manager.c list_manager.h list.h
	gcc $(OFLAGS) list_manager.c

clean:
	rm -f *.o *.s *.out