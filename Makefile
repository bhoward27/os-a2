CFLAGS = -Wall -Werror -std=c11 -D _POSIX_C_SOURCE=200809L -Werror
OFLAGS = -Wall -Werror -std=c11 -D _POSIX_C_SOURCE=200809L -g -c -Og

all: s-talk

run: s-talk
	./s-talk

s-talk: main.o list.o node_manager.o list_manager.o
	gcc $(CFLAGS) main.o list.o node_manager.o list_manager.o -lpthread -o s-talk

main.o: main.c
	gcc $(OFLAGS) main.c

list.o: list.c list.h node_manager.h list_manager.h
	gcc $(OFLAGS) list.c

node_manager.o: node_manager.c node_manager.h list.h
	gcc $(OFLAGS) node_manager.c

list_manager.o: list_manager.c list_manager.h list.h
	gcc $(OFLAGS) list_manager.c

clean:
	rm -f *.o *.s *.out s-talk