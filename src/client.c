#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define IP "127.0.0.1"
#define PORT 25575

int main() {
	int client = socket(AF_INET, SOCK_STREAM, 0);

	if (client == -1) {
		perror("Error");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in client_addr = {0};
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(PORT);

	int res;
	socklen_t addr_len = sizeof(client_addr);

	res = connect(client, (struct sockaddr*) &client_addr, addr_len);

	if (res == -1) {
		perror("Connect failed");
		exit(EXIT_FAILURE);
	}

	ssize_t nread;
	char buffer[256];
	char message[256];

	while (true) {
		printf("> ");
		fgets(message, sizeof(message), stdin);
		message[strcspn(message, "\n")] = '\0';
		write(client, message, strlen(message));
	}

	close(client);

	return 0;
}
