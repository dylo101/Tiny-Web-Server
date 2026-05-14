#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Minimal HTTP response header to send a 200 OK header
void send_ok_header(int client)
{
    const char *hdr = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html\r\n"
                      "\r\n";
    send(client, hdr , strlen(hdr), 0);
}

// Send the file body
void send_file(int client, const char *path)
{
    FILE *f = fopen(path, "r");
    if(!f)
    {
        perror("open html");
        return;
    }
    char buf[BUFFER_SIZE];
    size_t n;
    while((n = fread(buf, 1, sizeof buf, f)) > 0) 
    {
        send(client, buf, n, 0);
    }
    fclose(f);
}

int main() 
{
    // Create an IPv4 TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    // Prevents "address already in use" on quick restarts
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind to 0.0.0.0:8080
    // Define where the server listens
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;    // any local interface
    addr.sin_port = htons(PORT);          // host to network byte order

    if(bind(server_fd, (struct sockaddr*)&addr, sizeof addr) < 0)
    {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    // Start listening 
    // Queue incoming conenctions
    if(listen(server_fd, 5) < 0)
    {
        perror("listen");
        close(server_fd);
        exit(1);
    }
    printf("Listening on http://localhost:%d\n", PORT);

    // Accept one client (loop next)
    // Add an infinite loop to accept clients
    for (;;)
    {
        struct sockaddr_in cli;
        socklen_t len = sizeof cli;
        int client = accept(server_fd, (struct sockaddr*)&cli, &len);
        if(client < 0)
        {
            perror("accept");
            continue;
        }
        char request[BUFFER_SIZE];
        recv(client, request, sizeof(request) - 1, 0);
        
        printf("Client connected\n");
        send_ok_header(client);
        send_file(client, "index.html");
        printf("Client disconnected\n");
        // Every new connection gets headers + file
        close(client);
    }
}

