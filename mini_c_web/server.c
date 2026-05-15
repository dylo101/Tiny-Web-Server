#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_404(int client)
{
    const char *body = "<h1>404 Not Found</h1>";

    char hdr[256];
    snprintf(hdr, sizeof(hdr),
             "HTTP/1.1 404 Not Found\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n",
             strlen(body));

    send(client, hdr, strlen(hdr), 0);
    send(client, body, strlen(body), 0);
}

// Minimal HTTP response header to send a 200 OK header
void send_ok_header(int client)
{
    const char *hdr = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html\r\n"
                      "\r\n";
    send(client, hdr , strlen(hdr), 0);
}

// Send the file body
void send_file(int client, FILE *f)
{
    char buf[BUFFER_SIZE];
    size_t n;

    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
    {
        send(client, buf, n, 0);
    }
}

void get_requested_path(const char *request, char *path, size_t path_size)
{
    char method[16];
    char url[256];

    sscanf(request, "%15s %255s", method, url);

    if (strcmp(url, "/") == 0)
    {
        snprintf(path, path_size, "index.html");
    }
    else
    {
        // Skip the leading '/'
        snprintf(path, path_size, ".%s", url);
    }
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
        int bytes = recv(client, request, sizeof(request) - 1, 0);

        if(bytes <= 0)
        {
            close(client);
            continue;
        }
        request[bytes] = '\0';
        printf("Request:\n%s\n", request);

        char path[512];
        get_requested_path(request, path, sizeof(path));
        printf("Requested file: %s\n", path); 
          
        printf("Client connected\n");

        FILE *f = fopen(path, "rb");

        if(!f)
        {
            send_404(client);
            close(client);
            printf("Client disconnected\n");
            continue;
        }

        send_ok_header(client);
        send_file(client, f);

        fclose(f);

        printf("Client disconnected\n");
        close(client);
    }
}

