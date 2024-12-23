#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define IP "127.0.0.1"
#define PORT 25575

struct client_data {
	int client;
	struct sockaddr_in client_addr;
};

void* client_handle(void* arg) {
	ssize_t nread;
	char buffer[256];
	struct client_data* data = arg;
	struct in_addr addr = data->client_addr.sin_addr;
	char* client_ip = inet_ntoa(addr);
	unsigned short client_port = ntohs(data->client_addr.sin_port);
	char client_port_str[6];

	snprintf(client_port_str, sizeof(client_port_str), "%u", client_port);

	while (true) {
		nread = read(data->client, buffer, sizeof(buffer));

		if (nread <= 0) {
			break;
		}

		write(STDOUT_FILENO, client_ip, strlen(client_ip));
		write(STDOUT_FILENO, client_port_str, strlen(client_port_str));
		write(STDOUT_FILENO, ":", 1);
		write(STDOUT_FILENO, " > ", 3);
		write(STDOUT_FILENO, buffer, nread);
		write(STDOUT_FILENO, "\n", 1);
	}

	printf("%s:%d disconnected\n", client_ip, client_port);
	close(data->client);
	free(data);

	return NULL;
}

int main() {
	printf("Server starting...\n");

	int server = socket(AF_INET, SOCK_STREAM, 0);

	if (server == -1) {
		perror("Error");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_addr = {0};
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP);
	server_addr.sin_port = htons(PORT);

	int res;
	socklen_t addr_len = sizeof(server_addr);

	res = bind(server, (struct sockaddr*) &server_addr, addr_len);

	if (res == -1) {
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}

	printf("IP: %s\n", IP);
	printf("PORT: %d\n", PORT);

	res = listen(server, 5);

	if (res == -1) {
		perror("Listen failed: ");
		exit(EXIT_FAILURE);
	}

	printf("Server started!\n");

	struct sockaddr_in client_addr = {0};
	addr_len = sizeof(client_addr);

	int client;

	while (true) {
		client = accept(server, (struct sockaddr*) &client_addr, &addr_len);
		struct in_addr addr = client_addr.sin_addr;
		char* client_ip = inet_ntoa(addr);
		int client_port = ntohs(client_addr.sin_port);

		printf("%s:%d connected\n", client_ip, client_port);

		if (client == -1) {
			perror("Accept failed: ");
			exit(EXIT_FAILURE);
		}

		struct client_data* data = malloc(sizeof(struct client_data));

		data->client = client;
		data->client_addr = client_addr;

		pthread_t thread;

		pthread_create(&thread, NULL, client_handle, data);
	}

	close(server);

	return 0;
}
