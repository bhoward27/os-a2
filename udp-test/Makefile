OFLAGS = -Wall -Werror -std=c11 -D _POSIX_C_SOURCE=200809L -g -Og

all: receiver sender

receiver: receiver.c
	gcc $(OFLAGS) receiver.c -o receiver

sender: sender.c
	gcc $(OFLAGS) sender.c -o sender

clean:
	rm -f *.o *.s *.out receiver sender