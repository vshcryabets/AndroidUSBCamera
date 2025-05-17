#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

#define SOCKET_PATH "/tmp/unix_socket_example"

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[256];

    // Create socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    // Ensure the socket path is removed if it exists
    unlink(SOCKET_PATH);

    // Configure socket address
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    // Bind socket to the file path
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    // Listen for connections
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on " << SOCKET_PATH << std::endl;

    // Accept client connection
    client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    // Receive data
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    } else {
        perror("read");
    }

    // Clean up
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
