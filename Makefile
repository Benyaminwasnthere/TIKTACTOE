all: client server
client: client.c
	gcc -g -Wall -Werror -fsanitize=address client.c -o client
server: server.c
	gcc -g -Wall -Werror -fsanitize=address server.c -o server
clean:
	rm -rf client server
