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

    int serverFD = socket(AF_INET, SOCK_STREAM, 0);

    if (serverFD == -1) {
        std::cout << "Error creating socket" << std::endl;
        return 1;
    }

    int opt = 1;

    if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "Error setting SO_REUSEADDR" << std::endl;
        close(serverFD);
        return 1;
    }


    sockaddr_in server_adress{};
    server_adress.sin_family = AF_INET;
    server_adress.sin_addr.s_addr = INADDR_ANY;
    server_adress.sin_port = htons(5000);

    if (bind(serverFD, reinterpret_cast<sockaddr*>(&server_adress), sizeof(server_adress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverFD);
        return 1;
    }

    if (listen(serverFD, 5) == -1) {
        std::cerr << "Error listening socket" << std::endl;
        close(serverFD);
        return 1;
    }

    sockaddr_in clientAddress{};
    socklen_t clientAddressSize = sizeof(clientAddress);

    int clientFD = accept(serverFD, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
    if (clientFD == -1) {
        std::cerr << "Error accepting socket" << std::endl;
        close(serverFD);
        return 1;
    }


    std::string receivedMessage;

    if (!recvLine(clientFD, receivedMessage)) {
        close(clientFD);
        close(serverFD);
        return 1;
    }
    std::cout << "Client : "<<receivedMessage << std::endl;

    const char* reply = "Mesajin alindi!\n";

    if (!sendAll(clientFD, reply)) {
        close(clientFD);
        close(serverFD);
        return 1;
    }


    close(clientFD);
    close(serverFD);





    return 0;
}
