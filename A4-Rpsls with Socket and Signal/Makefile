CFLAGS = -g -Wall -std=gnu99

all: rpsls_server rpsls_client guess

rpsls_client: rpsls_client.o socket.o
	gcc ${CFLAGS} -o $@ $^

rpsls_server: rpsls_server.o socket.o
	gcc ${CFLAGS} -o $@ $^

guess: guess.o
	gcc ${CFLAGS} -o $@ $^

%.o: %.c socket.h
	gcc ${CFLAGS} -c $<

clean:
	rm *.o rpsls_client rpsls_server guess
