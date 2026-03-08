// Simple HTTP file server
//ip = go to cmd type ipconfig in that find Wireless LAN adapter Wi-Fi:  IPv4 Address. 
// Access via http://<ip>:12345/ to download the file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_fd;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);

    WSAStartup(MAKEWORD(2,2), &wsa);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);   // Port number
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);

    printf("Waiting for connection...\n");
    client_fd = accept(server_fd, (struct sockaddr*)&addr, &addrlen);
    printf("Client connected!\n");

    char buffer[8192];
    recv(client_fd, buffer, sizeof(buffer), 0); // receive HTTP request

    // Open file (change filename/extension as needed)
    FILE *fp = fopen("movie.avi", "rb");
    if (!fp) {
        printf("File not found!\n");
        return 1;
    }

    // Get file size for Content-Length
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Send HTTP headers
    char headers[512];
    sprintf(headers,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: video/x-msvideo\r\n"
        "Content-Disposition: attachment; filename=\"movie.avi\"\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n\r\n", filesize);

    send(client_fd, headers, strlen(headers), 0);

    // Send file content with larger buffer
    char filebuf[65536]; // 64 KB buffer
    int bytes;
    while ((bytes = fread(filebuf, 1, sizeof(filebuf), fp)) > 0) {
        send(client_fd, filebuf, bytes, 0);
    }

    fclose(fp);
    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();

    printf("Video sent!\n");
    return 0;

}
