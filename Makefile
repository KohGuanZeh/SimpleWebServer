webserver: webserver.o
	gcc src/webserver.o -lws2_32 -o webserver

webserver.o: src/webserver.c
	gcc -c src/webserver.c -o src/webserver.o