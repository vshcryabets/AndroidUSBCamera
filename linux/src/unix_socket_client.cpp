#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

#define SOCKET_PATH "/tmp/unix_socket_example"

int main() {
    int sock_fd;
    struct sockaddr_un addr;
    const char* message = "Hello from client!";

    // Create socket
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return 1;
    }

    // Configure socket address
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    // Connect to the server
    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(sock_fd);
        return 1;
    }

    // Send data
    if (write(sock_fd, message, strlen(message)) == -1) {
        perror("write");
    } else {
        std::cout << "Message sent: " << message << std::endl;
    }

    // Clean up
    close(sock_fd);

    return 0;
}
