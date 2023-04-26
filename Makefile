all: server client
server: ttts.c
	gcc -g -w -fsanitize=address ttts.c -o ttts
client: ttt.c
	gcc -g -w -fsanitize=address ttt.c -o ttt
clean:
	rm -f ttts ttt