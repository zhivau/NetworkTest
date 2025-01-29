#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break;
        }
    }
    close(client_socket);
}

void start_server() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Server socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        exit(1);
    }

    std::cout << "Server started at " << SERVER_IP << ":" << PORT << ", waiting for connections..." << std::endl;

    while (true) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket < 0) {
            perror("Client connection failed");
            continue;
        }

        std::thread(handle_client, client_socket).detach();
    }

    close(server_socket);
}

int main() {
    start_server();
    return 0;
}
