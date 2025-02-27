#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

int main() {
    WSADATA wsa_data{};
    WSAStartup(MAKEWORD(2, 2), &wsa_data);

    addrinfo* result{};
    addrinfo hints{};
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(nullptr, "8080", &hints, &result);

    SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);

    freeaddrinfo(result);

    listen(listen_socket, SOMAXCONN);

    SOCKET client_socket = accept(listen_socket, nullptr, nullptr);

    closesocket(listen_socket);

    char buffer[512]{};

    while(true) {
        auto recv_bytes = recv(client_socket, buffer, sizeof(buffer), 0);
        if(recv_bytes > 0) {
            auto send_bytes = send(client_socket, buffer, recv_bytes, 0);
        }
        else {
            break;
        }
    }

    shutdown(client_socket, SD_SEND);

    closesocket(client_socket);
    WSACleanup();

    return 0;
}