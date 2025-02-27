#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

int main() {
    WSADATA wsa_data{};

    WSAStartup(MAKEWORD(2, 2), &wsa_data);

    addrinfo* result;
    addrinfo hints{};
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    getaddrinfo("", "8080", &hints, &result);

    SOCKET connect_socket = INVALID_SOCKET;
    for(addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        auto res = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if(res == SOCKET_ERROR) {
            closesocket(connect_socket);
            connect_socket = INVALID_SOCKET;
            continue;
        }
        else {
            break;
        }
    }

    freeaddrinfo(result);

    const char* send_message = "test message";
    char buffer[512]{};
    send(connect_socket, send_message, (int)strlen(send_message), 0);

    shutdown(connect_socket, SD_SEND);

    while(true) {
        auto recv_bytes = recv(connect_socket, buffer, sizeof(buffer), 0);
        if(recv_bytes > 0) {

        }
        else {
            break;
        }
    }

    closesocket(connect_socket);
    WSACleanup();

    return 0;
}