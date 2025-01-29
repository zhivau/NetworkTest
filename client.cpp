#include <iostream>
#include <fstream>
#include <chrono>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

void test_network_speed(int num_packets, int packet_size, const std::string& filename) {
    int client_socket;
    struct sockaddr_in server_addr;
    char* buffer = new char[packet_size];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Client socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    std::cout << "Testing network speed...\n";
    std::ofstream outfile;
    outfile.open(filename, std::ios_base::app);
    outfile << "Testing with Packet Size: " << packet_size << " bytes, Num Packets: " << num_packets << "\n";

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_packets; ++i) {
        auto packet_start = std::chrono::high_resolution_clock::now();
        ssize_t bytes_sent = send(client_socket, buffer, packet_size, 0);
        if (bytes_sent == -1) {
            perror("Send failed");
            exit(1);
        }
        auto packet_end = std::chrono::high_resolution_clock::now();

        double packet_time = std::chrono::duration<double>(packet_end - packet_start).count() * 1000;
        std::cout << "Packet " << (i + 1) << " sent in " << packet_time << " ms\n";

        outfile << "Packet " << (i + 1) << " Time: " << packet_time << " ms\n";
    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed_time = std::chrono::duration<double>(end - start).count();
    double speed = (num_packets * packet_size) / elapsed_time / 1024 / 1024;
    double latency = elapsed_time / num_packets * 1000;

    std::cout << "\nTest finished!\n";
    std::cout << "Total Time: " << elapsed_time << " seconds\n";
    std::cout << "Speed: " << speed << " MB/s\n";
    std::cout << "Average Latency: " << latency << " ms\n";

    outfile << "\nTotal Time: " << elapsed_time << " seconds\n";
    outfile << "Speed: " << speed << " MB/s\n";
    outfile << "Average Latency: " << latency << " ms\n\n";

    outfile.close();
    close(client_socket);
    delete[] buffer;
}

int main(int argc, char* argv[]) {
    int num_packets = 50;
    int packet_size = 512;

    if (argc > 3) {
        std::cerr << "Usage: " << argv[0] << " [num_packets] [packet_size]\n";
        return 1;
    }

    if (argc > 1) {
        num_packets = std::stoi(argv[1]);
    }

    if (argc > 2) {
        packet_size = std::stoi(argv[2]);
    }

    std::string filename = "result.txt";
    test_network_speed(num_packets, packet_size, filename);

    return 0;
}
