SRC_DIR = "src"

all: clean server client

server:
	gcc $(SRC_DIR)/server.c -o $@

client:
	gcc $(SRC_DIR)/client.c -o $@

clean:
	rm -f server
	rm -f client