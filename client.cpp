#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

bool sendAll(int socketFD, const std::string& message) {
    std::size_t total_send = 0;

    while (total_send < message.length()) {

        ssize_t sent = send(socketFD, message.data() + total_send, static_cast<std::size_t>(message.length() - total_send), 0);

        if (sent == -1) {
            std::cout << "Error writing to socket" << std::endl;
            return false;
        }

        if (sent == 0) {
            std::cout << "Server closed the connection" << std::endl;
            return false;
        }

        total_send += static_cast<std::size_t>(sent);

    }
    return true;
}

bool recvLine(int socketFD, std::string& message) {

    constexpr std::size_t CHUNK_SIZE = 8;
    constexpr std::size_t MAX_MESSAGE_SIZE = 1024;

    char buffer[CHUNK_SIZE];

    while (true) {

        ssize_t receivedBytes = recv(socketFD, buffer, CHUNK_SIZE, 0);

        if (receivedBytes == -1) {
            std::cout << "Error reading from socket" << std::endl;
            return false;
        }
        if (receivedBytes == 0) {
            std::cout << "Server closed the connection" << std::endl;
            return false;
        }

        std::cout <<"recv() chunk size"<< receivedBytes << std::endl;

        message.append(buffer, static_cast<std::size_t>(receivedBytes));

        std::size_t newLinePos = message.find('\n');

        if (newLinePos != std::string::npos) {

            if (newLinePos > MAX_MESSAGE_SIZE) {
                std::cout << "Message too long" << std::endl;
                return false;
            }

            message.resize(newLinePos);
            return true;

        }
        if (message.size() > MAX_MESSAGE_SIZE) {
            std::cout << "Message too long" << std::endl;
            return false;
        }
    }
}



int main() {


    int clientFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFD == -1) {
        std::cout << "Error creating socket" << std::endl;
        return 1;
    };


    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5000);

    int ipResult =inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
    if (ipResult != 1) {
        std::cerr << "Error creating IP address" << std::endl;
        close(clientFD);
        return 1;
    }


    if (connect(clientFD, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::cout << "Error connecting to socket" << std::endl;
        close(clientFD);
        return 1;
    }

    const std::string message = "Hello from client\n";


    if (!sendAll(clientFD, message)) {
        close(clientFD);
        return 1;
    }

    std::string reply;

    if (!recvLine(clientFD, reply)) {
        close(clientFD);
        return 1;
    }
    std::cout << "server : "<<reply << std::endl;

    close(clientFD);

    return 0;
}
