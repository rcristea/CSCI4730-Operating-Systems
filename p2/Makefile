CC = gcc
CFLAGS = -g -pthread

all: webserver client webserver_multi

webserver: webserver.c net.c webserver.h
	$(CC) $(CFLAGS) -o $@ webserver.c net.c

webserver_multi: webserver_multi.c net.c webserver.h
	$(CC) $(CFLAGS) -o $@ webserver_multi.c net.c

client: client.c
	$(CC) $(CFLAGS) -o $@ client.c

clean:
	rm -f webserver client webserver_old webserver_multi
