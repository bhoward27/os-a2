CFLAGS = -Wall -Werror -std=c11 -D _POSIX_C_SOURCE=200809L -Werror
OFLAGS = -Wall -Werror -std=c11 -D _POSIX_C_SOURCE=200809L -g -c -Og

all: s-talk

run: s-talk
	./s-talk

s-talk: main.o list.o node_manager.o list_manager.o keyboard_receiver.o message_sender.o message_receiver.o printer.o sleep.o
	gcc $(CFLAGS) main.o list.o node_manager.o list_manager.o keyboard_receiver.o message_sender.o message_receiver.o printer.o sleep.o -lpthread -o s-talk

main.o: main.c list.h keyboard_receiver.h
	gcc $(OFLAGS) main.c

list.o: list.c list.h node_manager.h list_manager.h
	gcc $(OFLAGS) list.c

node_manager.o: node_manager.c node_manager.h list.h
	gcc $(OFLAGS) node_manager.c

list_manager.o: list_manager.c list_manager.h list.h
	gcc $(OFLAGS) list_manager.c

keyboard_receiver.o: keyboard_receiver.c keyboard_receiver.h list.h
	gcc $(OFLAGS) keyboard_receiver.c

message_receiver.o: message_receiver.c message_receiver.h list.h
	gcc $(OFLAGS) message_receiver.c

message_sender.o: message_sender.c message_sender.h list.h
	gcc $(OFLAGS) message_sender.c

printer.o: printer.c printer.h list.h
	gcc $(OFLAGS) printer.c

sleep.o: sleep.c sleep.h
	gcc $(OFLAGS) sleep.c

clean:
	rm -f *.o *.s *.out s-talk