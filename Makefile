CFLAGS = -Wall -Werror -std=c11 -D _POSIX_C_SOURCE=200809L -Werror
OFLAGS = -Wall -Werror -std=c11 -D _POSIX_C_SOURCE=200809L -g -c -Og

all: s-talk.out

run: s-talk.out
	./s-talk.out


s-talk.out: main.o list.o node_manager.o list_manager.o general.o receiver.o
	gcc $(CFLAGS) main.o list.o node_manager.o list_manager.o general.o receiver.o -lpthread -o s-talk.out

main.o: main.c
	gcc $(OFLAGS) main.c

list.o: list.c list.h node_manager.h list_manager.h
	gcc $(OFLAGS) list.c

node_manager.o: node_manager.c node_manager.h list.h
	gcc $(OFLAGS) node_manager.c

list_manager.o: list_manager.c list_manager.h list.h
	gcc $(OFLAGS) list_manager.c

general.o: general.c general.h
	gcc $(OFLAGS) general.c

receiver.o: receiver.c receiver.h
	gcc $(OFLAGS) receiver.c

clean:
	rm -f *.o *.s *.out